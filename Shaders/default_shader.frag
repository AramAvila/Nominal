#version 330 core
out vec4 FragColor;

in VS_OUT {
    vec3 FragPos;
    vec3 Normal;
    vec2 TexCoords;
    vec4 FragPosLightSpace;
    flat int sel;
} fs_in;

uniform sampler2D diffuseTexture;
uniform sampler2D shadowMap;
uniform samplerCube cubeDepthMap1;
uniform samplerCube cubeDepthMap2;
uniform samplerCube cubeDepthMap3;
uniform samplerCube cubeDepthMap4;
uniform samplerCube cubeDepthMap5;


uniform vec3 pointLightPos[5];
uniform vec3 pointLightColor[5];

uniform vec3 starLightPos;
uniform vec3 starLightColor;
uniform vec3 viewPos;

uniform vec3 debugChunkColor;
uniform int debugging;

uniform float far_plane;


// array of offset direction for sampling
vec3 gridSamplingDisk[20] = vec3[]
(
   vec3(1, 1, 1), vec3(1, -1, 1), vec3(-1, -1, 1), vec3(-1, 1, 1), 
   vec3(1, 1, -1), vec3(1, -1, -1), vec3(-1, -1, -1), vec3(-1, 1, -1),
   vec3(1, 1, 0), vec3(1, -1, 0), vec3(-1, -1, 0), vec3(-1, 1, 0),
   vec3(1, 0, 1), vec3(-1, 0, 1), vec3(1, 0, -1), vec3(-1, 0, -1),
   vec3(0, 1, 1), vec3(0, -1, 1), vec3(0, -1, -1), vec3(0, 1, -1)
);


float pointShadowCalculation(vec3 fragPos, samplerCube cubeDepthMap, vec3 LightPos)
{
    // Get vector between fragment position and light position
    vec3 fragToLight = fragPos - LightPos;
    float currentDepth = length(fragToLight);
    float shadow = 0.0;
    float bias = 0.15;
    int samples = 20;
    float viewDistance = length(viewPos - fragPos);
    float diskRadius = (1.0 + (viewDistance / far_plane)) / 25.0;
    for(int i = 0; i < samples; ++i)
    {
        float closestDepth = texture(cubeDepthMap, fragToLight + gridSamplingDisk[i] * diskRadius).r;
        closestDepth *= far_plane;   // Undo mapping [0;1]
        if(currentDepth - bias > closestDepth)
            shadow += 1.0;
    }
    shadow /= float(samples);
    return shadow;
}

float starShadowCalculation(vec4 fragPosLightSpace)
{
    // perform perspective divide
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    // Transform to [0,1] range
    projCoords = projCoords * 0.5 + 0.5;
    // Get closest depth value from light's perspective (using [0,1] range fragPosLight as coords)
    float closestDepth;
    closestDepth = texture(shadowMap, projCoords.xy).r;
    // Get depth of current fragment from light's perspective
    float currentDepth = projCoords.z;
    // Check whether current frag pos is in shadow
    float bias = 0.0005;
    float shadow = currentDepth - bias > closestDepth  ? 1.0 : 0.0;
    // Keep the shadow at 0.0 when outside the far_plane region of the light's frustum.
    if(projCoords.z > 1.0)
        shadow = 0.0;

    return shadow;
}

float specularValue(vec3 lightP){

    vec3 normal = normalize(fs_in.Normal);
    vec3 lightDir = normalize(lightP - fs_in.FragPos);
    // Specular
    vec3 viewDir = normalize(viewPos - fs_in.FragPos);
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = 0.0;
    vec3 halfwayDir = normalize(lightDir + viewDir);  
    spec = pow(max(dot(normal, halfwayDir), 0.0), 32.0);
    return spec;
}

float colorAttenuation(vec3 fragmentPos, vec3 lightPosition){
    float dist = distance(fragmentPos, lightPosition);
    float radius = 25;
    float att = max(0, 1 - (dist*dist)/(radius*radius));
    return att;
}

float onLight(vec3 lightPos){

    vec3 normal = normalize(fs_in.Normal);
    vec3 lightDir = normalize(lightPos - fs_in.FragPos);
    float reflectDir = dot(lightDir, normal);

    float light = 1;
    if(reflectDir < 0){
        light = 0;
    }

    return light;
}

void main()
{        
    vec3 color = texture(diffuseTexture, fs_in.TexCoords).rgb;
    // Calculate shadow
    float shadow1 = pointShadowCalculation(fs_in.FragPos, cubeDepthMap1, pointLightPos[0]);
    float shadow2 = pointShadowCalculation(fs_in.FragPos, cubeDepthMap2, pointLightPos[1]);
    float shadow3 = pointShadowCalculation(fs_in.FragPos, cubeDepthMap3, pointLightPos[2]);
    float shadow4 = pointShadowCalculation(fs_in.FragPos, cubeDepthMap4, pointLightPos[3]);
    float shadow5 = pointShadowCalculation(fs_in.FragPos, cubeDepthMap5, pointLightPos[4]);

    float shadowStar = starShadowCalculation(fs_in.FragPosLightSpace);

    float attenuations[5];
    for(int a = 0; a<5; a++){
        attenuations[a] = colorAttenuation(fs_in.FragPos, pointLightPos[a]);
    }

    vec3 starColorMod = color * starLightColor;

    vec3 lighting = ((1.0 - shadowStar) * (starColorMod * specularValue(starLightPos) + starColorMod/1.2 * onLight(starLightPos)));
    lighting += (1.0 - shadow1) * (specularValue(pointLightPos[0]) * color * pointLightColor[0] + attenuations[0] * color * pointLightColor[0]);
    lighting += (1.0 - shadow2) * (specularValue(pointLightPos[1]) * color * pointLightColor[1] + attenuations[1] * color * pointLightColor[1]);
    lighting += (1.0 - shadow3) * (specularValue(pointLightPos[2]) * color * pointLightColor[2] + attenuations[2] * color * pointLightColor[2]);
    lighting += (1.0 - shadow4) * (specularValue(pointLightPos[3]) * color * pointLightColor[3] + attenuations[3] * color * pointLightColor[3]);
    lighting += (1.0 - shadow5) * (specularValue(pointLightPos[4]) * color * pointLightColor[4] + attenuations[4] * color * pointLightColor[4]);

    vec3 colorOut = min(lighting, color);

    colorOut = fs_in.sel == 0 ? colorOut : colorOut*1.5;

    FragColor = vec4(colorOut, 1.0f);

    if(debugging != 0){
        FragColor = vec4(debugChunkColor, 1.0f);
    }
}