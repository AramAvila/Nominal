#version 330 core
out vec4 FragColor;

in VS_OUT {
    vec3 FragPos;
    vec3 Normal;
    vec2 TexCoords;
    vec4 FragPosLightSpace;
} fs_in;

uniform sampler2D diffuseTexture;
uniform sampler2D shadowMap;
uniform sampler2D shadowMap1;
uniform sampler2D shadowMap2;

uniform vec3 lightPos[4];
uniform vec3 lightColor[4];
uniform vec3 viewPos;

uniform bool shadows;

float ShadowCalculation(vec4 fragPosLightSpace, int i)
{
    // perform perspective divide
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    // Transform to [0,1] range
    projCoords = projCoords * 0.5 + 0.5;
    // Get closest depth value from light's perspective (using [0,1] range fragPosLight as coords)
    float closestDepth;
    if(i==0){
        closestDepth = texture(shadowMap, projCoords.xy).r;
    } else if(i==1){
        closestDepth = texture(shadowMap1, projCoords.xy).r;
    }else if(i==2){
        closestDepth = texture(shadowMap2, projCoords.xy).r;
    }else if(i==3){
        closestDepth = texture(shadowMap3, projCoords.xy).r;
    }else if(i==4){
        closestDepth = texture(shadowMap4, projCoords.xy).r;
    }
    // Get depth of current fragment from light's perspective
    float currentDepth = projCoords.z;
    // Check whether current frag pos is in shadow
    float bias = 0.001;
    float shadow = currentDepth - bias > closestDepth  ? 1.0 : 0.0; 

    return shadow;
} 

void main()
{     
    vec3 color = texture(diffuseTexture, fs_in.TexCoords).rgb;
    vec3 normal = normalize(fs_in.Normal);
    vec3 diffuse;
    vec3 specular;

    float shadow;
    vec3 lighting[3];

    for(int i = 0; i < 3; i++){
        vec3 lightDir = normalize(lightPos[i] - fs_in.FragPos);
        float diff = dot(lightDir, normal) < 0 ? 0 : 1;
        diffuse = diff * lightColor[i];
        // Specular
        vec3 viewDir = normalize(viewPos - fs_in.FragPos);
        float spec = 0.0;
        vec3 halfwayDir = normalize(lightDir + viewDir);  
        spec = pow(max(dot(normal, halfwayDir), 0.0), 64.0);
        specular = spec * lightColor[i];

        shadow = ShadowCalculation(fs_in.FragPosLightSpace[i], i);
        lighting[i] = (((1.0 - shadow) * (diffuse + specular)) * color);
    }

    vec3 defColor = lighting[0]*0.4 + lighting[1]*0.4 + lighting[2]*0.4;
    
    FragColor = vec4(defColor, 1.0f);
}