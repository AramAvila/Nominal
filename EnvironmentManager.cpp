#include "EnvironmentManager.h"

extern std::unique_ptr<Renderer> renderer;
extern std::vector<std::shared_ptr<ChunkPack>>* chunkPacks;
extern std::unique_ptr<EntityManager> entityManager;
extern glm::vec3 closestPoint; //This is the last closest point found, used in the renderer class to debug

extern std::unique_ptr<Camera> camera;

extern std::unique_ptr<Assets> assets;

EnvironmentManager::EnvironmentManager()
{
}

EnvironmentManager::~EnvironmentManager()
{
}

void EnvironmentManager::deleteModel(float x, float y, float z) {

	/* To delete a block we will change the reference system of the point for each chunk.
	*  Then, we will look for the closes block to that point in the pack
	*  If there is a block, it will be deleted.
	*/

	/*float dist = Configuration::MAXIMUM_JOIN_RANGE * 2;//just to make sure the condition will be valid
	std::vector<std::shared_ptr<Block>>::iterator closestBlock;
	std::vector<std::shared_ptr<Chunk>>::iterator closestChunk;
	std::vector<std::shared_ptr<ChunkPack>>::iterator closestPack;

	bool found = false;

	//This iteration finds the closest block to the new object location
	for (std::vector<std::shared_ptr<ChunkPack>>::iterator chunkPack = chunkPacks->begin(); chunkPack != chunkPacks->end(); chunkPack++)
	{
		glm::vec3 relPosition = MathHelper::changeReferenceSystem(glm::vec3(x, y, z), (*chunkPack)->absPosition, (*chunkPack)->relativeAxisX, (*chunkPack)->relativeAxisY, (*chunkPack)->relativeAxisZ);

		for (std::vector<std::shared_ptr<Chunk>>::iterator chunk = (*chunkPack)->chunkList.begin(); chunk != (*chunkPack)->chunkList.end(); chunk++)
		{
			//if the distance between the center of the chunk and the point is less than the lentgth of the diagonal of the chunk plus the join range of the cube, the block will be in range to join the chunk
			if (MathHelper::getVectorLength((glm::vec3)(*chunk)->getChunkCenter() - relPosition) < sqrt(3 * pow(Configuration::CHUNK_SIZE / 2, 2)) + Configuration::MAXIMUM_JOIN_RANGE) {
				//if we entered this loop is becouse the chunk is close enough to the block creation so that it may contain a block to join the new one
				//iterate through all of the blocks to find the closest one
				for (std::vector<std::shared_ptr<Block>>::iterator obj = (*chunk)->blockList.begin(); obj != (*chunk)->blockList.end(); obj++)
				{
					glm::vec3 posDiference = relPosition - (glm::vec3)(*obj)->relPosition;
					float newDist = MathHelper::getVectorLength(posDiference);

					if (newDist < Configuration::MAXIMUM_JOIN_RANGE) {

						if (newDist < dist) {
							found = true;
							dist = newDist;
							closestBlock = obj;
							closestChunk = chunk;
							closestPack = chunkPack;
						};
					}
				}
			}
		}
	}

	if (found) {	//if there is a block to delete, we will delete it
		(*closestBlock).reset();
		(*closestChunk)->blockList.erase(closestBlock);

		if ((*closestChunk)->blockList.size() == 0) { //if it was the only block in the chunk, the chunk will be deleted
			(*closestChunk).reset();
			(*closestPack)->chunkList.erase(closestChunk);

			if ((*closestPack)->chunkList.size() == 0) { //if the chunk was the last of it's pack, the pack will be deleted
				(*closestPack).reset();
				chunkPacks->erase(closestPack);
			}
		}
		else {
			(*closestChunk)->updateChunkVAO(); //otherwise, the chunk's VAO will be updated
		}
	}*/
}

void EnvironmentManager::loadStar(std::shared_ptr<GlobalLight> star)
{
	glUniform3fv(glGetUniformLocation(renderer->defaultShader.Program, "starLightPos"), 1, &star->absPosition[0]);
	glUniform3fv(glGetUniformLocation(renderer->defaultShader.Program, "starLightColor"), 1, &star->color[0]);

	glUniformMatrix4fv(glGetUniformLocation(renderer->defaultShader.Program, "starLightSpaceMatrix"), 1, GL_FALSE, glm::value_ptr(star->lightSpaceMatrix));
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, star->depthMap);
}

void EnvironmentManager::updateDepthMap(std::shared_ptr<GlobalLight> star)
{
	// 1. Render depth of scene to texture (from light's perspective)
	// - Get light projection/view matrix.
	glCullFace(GL_FRONT);
	glm::mat4 lightProjection, lightView;
	glm::mat4 lSM;
	lightProjection = glm::ortho(-50.0f, 50.0f, 50.0f, -50.0f, (GLfloat)Configuration::STAR_SHADOW_NEAR, (GLfloat)Configuration::STAR_SHADOW_FAR);
	//lightProjection = glm::perspective(45.0f, (GLfloat)Configuration::SHADOW_WIDTH / (GLfloat)Configuration::SHADOW_HEIGHT, near_plane, far_plane); // Note that if you use a perspective projection matrix you'll have to change the light absPosition as the current light absPosition isn't enough to reflect the whole scene.
	lightView = glm::lookAt(star->absPosition, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0, 0.0f));
	lSM = lightProjection * lightView;
	// - now render scene from light's point of view
	renderer->ortogonalDepthShader.Use();
	glUniformMatrix4fv(glGetUniformLocation(renderer->ortogonalDepthShader.Program, "lightSpaceMatrix"), 1, GL_FALSE, glm::value_ptr(lSM));
	glViewport(0, 0, Configuration::SHADOW_WIDTH, Configuration::SHADOW_HEIGHT);
	glBindFramebuffer(GL_FRAMEBUFFER, star->depthMapFBO);
	glClear(GL_DEPTH_BUFFER_BIT);
	renderer->renderScene(renderer->ortogonalDepthShader);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	star->lightSpaceMatrix = lSM;
	glCullFace(GL_BACK);
}

void EnvironmentManager::loadPointLight(std::shared_ptr<PointLight> light)
{
	glUniform3fv(glGetUniformLocation(renderer->defaultShader.Program, "pointLightPos"), 1, &light->absPosition[0]);
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_CUBE_MAP, light->depthCubeMap);
}

void EnvironmentManager::loadPointLightList(std::shared_ptr<PointLight> lights[5])
{
	for (int i = 0; i < 5; i++)
	{
		glUniform3fv(glGetUniformLocation(renderer->defaultShader.Program, ("pointLightPos[" + std::to_string(i) + "]").c_str()), 1, &lights[i]->absPosition[0]);
		glUniform3fv(glGetUniformLocation(renderer->defaultShader.Program, ("pointLightColor[" + std::to_string(i) + "]").c_str()), 1, &lights[i]->color[0]);
		glActiveTexture(GL_TEXTURE2 + i);
		glBindTexture(GL_TEXTURE_CUBE_MAP, lights[i]->depthCubeMap);
	}

}

void EnvironmentManager::updateDepthCubeMap(std::shared_ptr<PointLight> light)
{
	// 0. Create depth cubemap transformation matrices
	GLfloat aspect = (GLfloat)Configuration::POINT_LIGHT_SHADOW_WIDTH / (GLfloat)Configuration::POINT_LIGHT_SHADOW_HEIGHT;
	GLfloat near = 1.0f;
	GLfloat far = 25.0f;
	glm::mat4 shadowProj = glm::perspective(glm::radians(90.0f), aspect, near, far);
	std::vector<glm::mat4> shadowTransforms;
	shadowTransforms.push_back(shadowProj * glm::lookAt(light->absPosition, light->absPosition + glm::vec3(1.0, 0.0, 0.0), glm::vec3(0.0, -1.0, 0.0)));
	shadowTransforms.push_back(shadowProj * glm::lookAt(light->absPosition, light->absPosition + glm::vec3(-1.0, 0.0, 0.0), glm::vec3(0.0, -1.0, 0.0)));
	shadowTransforms.push_back(shadowProj * glm::lookAt(light->absPosition, light->absPosition + glm::vec3(0.0, 1.0, 0.0), glm::vec3(0.0, 0.0, 1.0)));
	shadowTransforms.push_back(shadowProj * glm::lookAt(light->absPosition, light->absPosition + glm::vec3(0.0, -1.0, 0.0), glm::vec3(0.0, 0.0, -1.0)));
	shadowTransforms.push_back(shadowProj * glm::lookAt(light->absPosition, light->absPosition + glm::vec3(0.0, 0.0, 1.0), glm::vec3(0.0, -1.0, 0.0)));
	shadowTransforms.push_back(shadowProj * glm::lookAt(light->absPosition, light->absPosition + glm::vec3(0.0, 0.0, -1.0), glm::vec3(0.0, -1.0, 0.0)));

	// 1. Render scene to depth cubemap
	glViewport(0, 0, Configuration::POINT_LIGHT_SHADOW_WIDTH, Configuration::POINT_LIGHT_SHADOW_HEIGHT);
	glBindFramebuffer(GL_FRAMEBUFFER, light->depthCubeMapFBO);
	glClear(GL_DEPTH_BUFFER_BIT);
	renderer->pointDepthShader.Use();
	for (GLuint i = 0; i < 6; ++i)
		glUniformMatrix4fv(glGetUniformLocation(renderer->pointDepthShader.Program, ("shadowTransforms[" + std::to_string(i) + "]").c_str()), 1, GL_FALSE, glm::value_ptr(shadowTransforms[i]));
	glUniform1f(glGetUniformLocation(renderer->pointDepthShader.Program, "far_plane"), far);
	glUniform3fv(glGetUniformLocation(renderer->pointDepthShader.Program, "lightPos"), 1, &light->absPosition[0]);
	renderer->renderScene(renderer->pointDepthShader);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void EnvironmentManager::setUpSkyBox() {

	// Set the object data (buffers, vertex attributes)
	GLfloat cubeVertices[] = {
		// Positions          // Texture Coords
		-0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
		0.5f, -0.5f, -0.5f,  1.0f, 0.0f,
		0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
		0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
		-0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, 0.0f,

		-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
		0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
		0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
		0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
		-0.5f,  0.5f,  0.5f,  0.0f, 1.0f,
		-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,

		-0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
		-0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
		-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
		-0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

		0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
		0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
		0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
		0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
		0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
		0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

		-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
		0.5f, -0.5f, -0.5f,  1.0f, 1.0f,
		0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
		0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
		-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,

		-0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
		0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
		0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
		0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
		-0.5f,  0.5f,  0.5f,  0.0f, 0.0f,
		-0.5f,  0.5f, -0.5f,  0.0f, 1.0f
	};
	GLfloat skyboxVertices[] = {
		// Positions          
		-1.0f,  1.0f, -1.0f,
		-1.0f, -1.0f, -1.0f,
		1.0f, -1.0f, -1.0f,
		1.0f, -1.0f, -1.0f,
		1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,

		-1.0f, -1.0f,  1.0f,
		-1.0f, -1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f,  1.0f,
		-1.0f, -1.0f,  1.0f,

		1.0f, -1.0f, -1.0f,
		1.0f, -1.0f,  1.0f,
		1.0f,  1.0f,  1.0f,
		1.0f,  1.0f,  1.0f,
		1.0f,  1.0f, -1.0f,
		1.0f, -1.0f, -1.0f,

		-1.0f, -1.0f,  1.0f,
		-1.0f,  1.0f,  1.0f,
		1.0f,  1.0f,  1.0f,
		1.0f,  1.0f,  1.0f,
		1.0f, -1.0f,  1.0f,
		-1.0f, -1.0f,  1.0f,

		-1.0f,  1.0f, -1.0f,
		1.0f,  1.0f, -1.0f,
		1.0f,  1.0f,  1.0f,
		1.0f,  1.0f,  1.0f,
		-1.0f,  1.0f,  1.0f,
		-1.0f,  1.0f, -1.0f,

		-1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f,  1.0f,
		1.0f, -1.0f, -1.0f,
		1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f,  1.0f,
		1.0f, -1.0f,  1.0f
	};

	// Setup cube VAO
	GLuint cubeVAO, cubeVBO;
	glGenVertexArrays(1, &cubeVAO);
	glGenBuffers(1, &cubeVBO);
	glBindVertexArray(cubeVAO);
	glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVertices), &cubeVertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
	glBindVertexArray(0);
	// Setup skybox VAO
	GLuint skyboxVAO, skyboxVBO;
	glGenVertexArrays(1, &skyboxVAO);
	glGenBuffers(1, &skyboxVBO);
	glBindVertexArray(skyboxVAO);
	glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
	glBindVertexArray(0);


	// Cubemap (Skybox)
	const GLchar* faces[5][6];
	faces[0][0] = "textures/SkyBoxes/Planet/PlanetLeft.png";
	faces[0][1] = "textures/SkyBoxes/Planet/PlanetRight.png";
	faces[0][2] = "textures/SkyBoxes/Planet/PlanetTop.png";
	faces[0][3] = "textures/SkyBoxes/Planet/PlanetBot.png";
	faces[0][4] = "textures/SkyBoxes/Planet/PlanetBack.png";
	faces[0][5] = "textures/SkyBoxes/Planet/PlanetFront.png";

	faces[1][0] = "textures/SkyBoxes/Nebulosa/nebulosaLeft.png";
	faces[1][1] = "textures/SkyBoxes/Nebulosa/nebulosaRight.png";
	faces[1][2] = "textures/SkyBoxes/Nebulosa/nebulosaTop.png";
	faces[1][3] = "textures/SkyBoxes/Nebulosa/nebulosaBot.png";
	faces[1][4] = "textures/SkyBoxes/Nebulosa/nebulosaBack.png";
	faces[1][5] = "textures/SkyBoxes/Nebulosa/nebulosaFront.png";

	faces[2][0] = "textures/SkyBoxes/Orbital/5dim_Right.png";
	faces[2][1] = "textures/SkyBoxes/Orbital/5dim_Left.png";
	faces[2][2] = "textures/SkyBoxes/Orbital/5dim_Up.png";
	faces[2][3] = "textures/SkyBoxes/Orbital/5dim_Down.png";
	faces[2][4] = "textures/SkyBoxes/Orbital/5dim_Back.png";
	faces[2][5] = "textures/SkyBoxes/Orbital/5dim_Front.png";

	faces[3][0] = "textures/SkyBoxes/Ships/classmplanet_rt.png";
	faces[3][1] = "textures/SkyBoxes/Ships/classmplanet_lf.png";
	faces[3][2] = "textures/SkyBoxes/Ships/classmplanet_up.png";
	faces[3][3] = "textures/SkyBoxes/Ships/classmplanet_dn.png";
	faces[3][4] = "textures/SkyBoxes/Ships/classmplanet_bk.png";
	faces[3][5] = "textures/SkyBoxes/Ships/classmplanet_ft.png";

	faces[4][0] = "textures/SkyBoxes/Stars/purplenebula_rt.png";
	faces[4][1] = "textures/SkyBoxes/Stars/purplenebula_lf.png";
	faces[4][2] = "textures/SkyBoxes/Stars/purplenebula_up.png";
	faces[4][3] = "textures/SkyBoxes/Stars/purplenebula_dn.png";
	faces[4][4] = "textures/SkyBoxes/Stars/purplenebula_bk.png";
	faces[4][5] = "textures/SkyBoxes/Stars/purplenebula_ft.png";

	GLuint textureID[5];
	int width = 1024;
	int height = 1024;

	for (int a = 0; a < 5; a++)
	{

		glGenTextures(1, &textureID[a]);

		unsigned char* image;

		glBindTexture(GL_TEXTURE_CUBE_MAP, textureID[a]);
		for (GLuint i = 0; i < 6; i++)
		{
			image = SOIL_load_image(faces[a][i], &width, &height, 0, SOIL_LOAD_RGB);
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
			SOIL_free_image_data(image);
		}
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
		glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

		this->skyBoxTexture[a] = textureID[a];
	}
	this->skyBoxVAO = skyboxVAO;
}

/*/// <summary>
/// Spawns a cube
/// </summary>
/// <param name="absPosition">The absolute position of the cube, needed if we have to spawn a new chunkPack.</param>
/// <param name="relPosition">The relative position: if the cube has to join another chunkpack.</param>
/// <param name="closestBlock">The closest block.</param>
/// <param name="closestChunk">The closest chunk.</param>
/// <param name="closestPack">The closest pack.</param>
/// <param name="id">The identifier of the cube.</param>
void EnvironmentManager::spawnCube(glm::vec3 absPosition, glm::vec3 relPosition, Entity* closestBlock, Chunk* closestChunk, ChunkPack* closestPack, std::shared_ptr<Block> cube)
{

	//if dist is less than Configuration::MAXIMUM_JOIN_RANGE * 2, it means that a close enough block has been found.
	if (closestBlock != NULL) {

		glm::ivec3 offset = MathHelper::getVectorDirection((glm::vec3)relPosition - glm::vec3(closestBlock->relPosition));

		glm::ivec3 newBlockRelPos = closestBlock->relPosition + offset;

		glm::vec3 a = (glm::vec3)newBlockRelPos / (float)Configuration::CHUNK_SIZE; //var a only used in the following 4 lines
		a.x = a.x < 0 ? a.x - 1 : a.x;
		a.y = a.y < 0 ? a.y - 1 : a.y; //first chunk in xPos will be 0,0,0 ----- first chunk in xNeg will be -1,0,0
		a.z = a.z < 0 ? a.z - 1 : a.z;
		glm::ivec3 newBlockChunk = (glm::ivec3)a;  //casting to integer will truncate the float values  12.35->12 , -15.247->-15

		std::vector<std::shared_ptr<Chunk>>::iterator chunk = closestPack->chunkList.begin();

		bool found = false;

		while (!found && chunk != closestPack->chunkList.end()) { //iterate through all chunks in the chunk pack to see if the one the block should be on already exists
			if ((*chunk)->relativePosition == newBlockChunk) {
				found = true;
			}
			else {
				chunk++;
			}
		}

		if (found) {
			cube->relPosition = newBlockRelPos;
			(*chunk)->blockList.push_back(std::move(cube));
			(*chunk)->updateChunkVAO();
		}
		else {

			cube->relPosition = closestBlock->relPosition + offset;

			std::shared_ptr<Chunk> newChunk(new Chunk(newBlockChunk.x, newBlockChunk.y, newBlockChunk.z));
			newChunk->blockList.push_back(std::move(cube));
			newChunk->updateChunkVAO();

			closestPack->chunkList.push_back(std::move(newChunk));
		}
	}
	else {//there is no chunk close enough so a new chunk pack will be created
		std::shared_ptr<ChunkPack> newPack(new ChunkPack());

		newPack->relativeAxisX = camera->Right;
		newPack->relativeAxisY = camera->Up;
		newPack->relativeAxisZ = -camera->Front;
		newPack->pitch = camera->Pitch;
		newPack->yaw = camera->Yaw;
		newPack->roll = camera->Roll;

		std::shared_ptr<Chunk> newChunk(new Chunk(0, 0, 0));

		newPack->absPosition = glm::vec3(absPosition);
		cube->relPosition = glm::vec3(0, 0, 0);

		newChunk->blockList.push_back(std::move(cube));
		newChunk->updateChunkVAO();

		newPack->chunkList.push_back(std::move(newChunk));
		chunkPacks->push_back(std::move(newPack));
	}

}


/// <summary>
/// Spawns a generator
/// </summary>
/// <param name="absPosition">The absolute position of the cube: needed if we have to spawn a new chunkPack.</param>
/// <param name="relPosition">The relative position: if the cube has to join another chunkpack.</param>
/// <param name="closestBlock">The closest block.</param>
/// <param name="closestChunk">The closest chunk.</param>
/// <param name="closestPack">The closest pack.</param>
/// <param name="point">The object "generator" to spawn.</param>
void EnvironmentManager::spawnGenerator(glm::vec3 absPosition, glm::vec3 relPosition, Entity* closestBlock, Chunk* closestChunk, ChunkPack* closestPack, std::shared_ptr<Generator> generator) {

	if (closestBlock != NULL) {

		glm::ivec3 offset = MathHelper::getVectorDirection((glm::vec3)relPosition - glm::vec3(closestBlock->relPosition));

		glm::ivec3 newBlockRelPos = closestBlock->relPosition + offset;

		//checking colision box
		bool clear = true;

		//snapping camera axis to chunk's axis
		glm::vec3 collisionXaxis = MathHelper::getVectorDirectionCustomAxis(camera->Front, closestPack->relativeAxisX, closestPack->relativeAxisY, closestPack->relativeAxisZ);
		glm::vec3 collisionYaxis = MathHelper::getVectorDirectionCustomAxis(camera->Up, closestPack->relativeAxisX, closestPack->relativeAxisY, closestPack->relativeAxisZ);
		glm::vec3 collisionZaxis = MathHelper::getVectorDirectionCustomAxis(camera->Right, closestPack->relativeAxisX, closestPack->relativeAxisY, closestPack->relativeAxisZ);

		generator->orientationX = collisionXaxis;
		generator->orientationY = collisionYaxis;
		generator->orientationZ = collisionZaxis;

		for (int cX = 0; cX <= generator->collisionBox.x; cX++)
		{
			for (int cY = 0; cY <= generator->collisionBox.y; cY++)
			{
				for (int cZ = 0; cZ <= generator->collisionBox.z; cZ++)
				{
					glm::ivec3 position = collisionXaxis * (float)cX + collisionYaxis * (float)cY + collisionZaxis * (float)cZ + (glm::vec3)newBlockRelPos;
					std::vector<std::shared_ptr<Chunk>>::iterator chunkIt = closestPack->chunkList.begin();
					while (clear && chunkIt != closestPack->chunkList.end()) {
						std::vector<std::shared_ptr<Block>>::iterator entIt = (*chunkIt)->blockList.begin();

						while (clear && entIt != (*chunkIt)->blockList.end()) {

							if ((*entIt)->relPosition == position) {
								clear = false;
							}
							entIt++;
						}
						chunkIt++;
					}
				}
			}
		}

		if (clear) {

			glm::vec3 a = (glm::vec3)newBlockRelPos / (float)Configuration::CHUNK_SIZE; //var a only used in the following 4 lines
			a.x = a.x < 0 ? a.x - 1 : a.x;
			a.y = a.y < 0 ? a.y - 1 : a.y; //first chunk in xPos will be 0,0,0 ----- first chunk in xNeg will be -1,0,0
			a.z = a.z < 0 ? a.z - 1 : a.z;
			glm::ivec3 newBlockChunk = (glm::ivec3)a;  //casting to integer will truncate the float values  12.35->12 , -15.247->-15

			std::vector<std::shared_ptr<Chunk>>::iterator chunk = closestPack->chunkList.begin();

			bool found = false;

			while (!found && chunk != closestPack->chunkList.end()) { //iterate through all chunks in the chunk pack to see if the one the block should be on already exists
				if ((*chunk)->relativePosition == newBlockChunk) {
					found = true;
				}
				else {
					chunk++;
				}
			}

			std::shared_ptr<Generator> copyForConnection = generator;

			if (found) {
				generator->relPosition = newBlockRelPos;
				generator->setUpProps();

				(*chunk)->generatorList.push_back(std::move(generator));
				(*chunk)->updateChunkVAO();
			}
			else {
				generator->relPosition = closestBlock->relPosition + offset;
				generator->setUpProps();

				std::shared_ptr<Chunk> newChunk(new Chunk(newBlockChunk.x, newBlockChunk.y, newBlockChunk.z));
				newChunk->generatorList.push_back(std::move(generator));
				newChunk->updateChunkVAO();

				closestPack->chunkList.push_back(std::move(newChunk));
			}

			std::shared_ptr<HelperClasses::Connection> newConnection(new HelperClasses::Connection());
			newConnection->type = copyForConnection->outputType;

			newConnection->wirePoints.push_back(copyForConnection->outputPoint);

			newConnection->generatorList.push_back(std::move(copyForConnection));
			closestPack->connectionList.push_back(std::move(newConnection));
		}
	}
}

void EnvironmentManager::spawnWire(glm::vec3 absPosition, glm::vec3 relPosition, WireHolder * closestWH, Entity * closestBlock, Chunk * closestChunk, ChunkPack * closestPack, std::shared_ptr<Wire> wire)
{

	glm::ivec3 offset = MathHelper::getVectorDirection((glm::vec3)relPosition - glm::vec3(closestBlock->relPosition));

	glm::ivec3 newBlockRelPos = closestBlock->relPosition + offset;

	//if we are spawning a wire at the same spot of the closest wire holder, the wire will be added to the wh.
	bool placed = false;
	if (closestWH != NULL && closestWH->position == newBlockRelPos) {

		if (closestWH->wireList[0] == NULL) {
			closestWH->wireList[0] = wire;
			placed = true;
		}
		else if (closestWH->wireList[1] == NULL) {
			closestWH->wireList[1] = wire;
			placed = true;
		}
		else if (closestWH->wireList[2] == NULL) {
			closestWH->wireList[2] = wire;
			placed = true;
		}
		else if (closestWH->wireList[3] == NULL) {
			closestWH->wireList[3] = wire;
			placed = true;
		}
	}

	if (!placed) {

		std::shared_ptr<WireHolder> newWH(new WireHolder());

		newWH->wireList[0] = wire;
		newWH->position = newBlockRelPos;

		std::vector<std::shared_ptr< WireHolder>> adjacentWH;
		std::vector<glm::ivec3> sides;

		for (std::vector<std::shared_ptr<Chunk>>::iterator chunk = closestPack->chunkList.begin(); chunk != closestPack->chunkList.end(); chunk++) {
			for (std::vector<std::shared_ptr<WireHolder>>::iterator wH = (*chunk)->wireHolderList.begin(); wH != (*chunk)->wireHolderList.end(); wH++) {
				if (MathHelper::getVectorLength((*wH)->position - newBlockRelPos) < 1.2f) {
					adjacentWH.push_back((*wH));
					sides.push_back(MathHelper::getVectorDirection((*wH)->position - newBlockRelPos));
				}
			}
		}

		if (sides.size() > 0) {
			for each (std::shared_ptr<WireHolder> wh in adjacentWH)
			{
				for each (glm::ivec3 side in sides)
				{
					wh->connectedFaces.push_back(side);
				}
				wh->updateConnectedModels();
			}

			for each (glm::ivec3 side in sides)
			{
				newWH->connectedFaces.push_back(-side);
			}
			newWH->updateConnectedModels();
		}

		closestChunk->wireHolderList.push_back(std::move(newWH));

	}

	closestChunk->updateChunkVAO();
	//here ends the visual part of the wire spawn. Now it's turn for the active side of it.

	std::vector<std::shared_ptr<HelperClasses::Connection>> connectionsToJoin;
	std::vector<int> indexesToRemove;

	for (int i = 0; i < closestPack->connectionList.size(); i++)
	{
		if (closestPack->connectionList.at(i)->type == wire->type) {
			bool found = false;
			std::vector<glm::ivec3>::iterator pos = closestPack->connectionList.at(i)->wirePoints.begin();

			while (!found && pos != closestPack->connectionList.at(i)->wirePoints.end()) {
				if (MathHelper::getVectorLength((*pos) - newBlockRelPos) < 1.2) {
					found = true;
					connectionsToJoin.push_back(std::move(closestPack->connectionList.at(i)));//shared pointers are removed from the chunkpack list
					closestPack->connectionList.at(i).reset();
					indexesToRemove.insert(indexesToRemove.begin(), i);
				}
				pos++;
			}
		}
	}

	for each (int index in indexesToRemove)
	{
		closestPack->connectionList.erase(closestPack->connectionList.begin() + index);
	}

	switch (connectionsToJoin.size()) {
	case 0: //if no other conection of the same type has benn found nearby, a new one will created and be appended to the chunkPack
	{
		std::shared_ptr<HelperClasses::Connection> newConnection(new HelperClasses::Connection());
		newConnection->type = wire->type;
		newConnection->wirePoints.push_back(newBlockRelPos);
		closestPack->connectionList.push_back(std::move(newConnection));
		break;
	}

	case 1: //if only one other connection of the same type has been found, the new wire will be appended to it
	{
		connectionsToJoin.at(0)->wirePoints.push_back(newBlockRelPos);
		closestPack->connectionList.push_back(std::move(connectionsToJoin.at(0)));

		break;
	}

	default: //if more than 1 connection of the same type has been found, all of them will be joined
	{
		std::vector<std::shared_ptr<HelperClasses::Connection>>::iterator conn = connectionsToJoin.begin();
		std::shared_ptr<HelperClasses::Connection> toAdd = std::move(connectionsToJoin.at(0));
		conn++;

		toAdd->wirePoints.push_back(newBlockRelPos);

		while (conn != connectionsToJoin.end()) {
			toAdd->generatorList.insert(toAdd->generatorList.end(), (*conn)->generatorList.begin(), (*conn)->generatorList.end());
			toAdd->wirePoints.insert(toAdd->wirePoints.end(), (*conn)->wirePoints.begin(), (*conn)->wirePoints.end());
			toAdd->engineList.insert(toAdd->engineList.end(), (*conn)->engineList.begin(), (*conn)->engineList.end());
			conn++;
		}
		closestPack->connectionList.push_back(std::move(toAdd));
		break;
	}
	}
}

void EnvironmentManager::spawnEngine(glm::vec3 absPosition, glm::vec3 relPosition, Entity * closestBlock, Chunk * closestChunk, ChunkPack * closestPack, std::shared_ptr<Engine> engine)
{
	if (closestBlock != NULL) {

		glm::ivec3 offset = MathHelper::getVectorDirection((glm::vec3)relPosition - glm::vec3(closestBlock->relPosition));

		glm::ivec3 newBlockRelPos = closestBlock->relPosition + offset;

		//checking colision box
		bool clear = true;

		//snapping camera axis to chunk's axis
		glm::vec3 collisionXaxis = MathHelper::getVectorDirectionCustomAxis(camera->Front, closestPack->relativeAxisX, closestPack->relativeAxisY, closestPack->relativeAxisZ);
		glm::vec3 collisionYaxis = MathHelper::getVectorDirectionCustomAxis(camera->Up, closestPack->relativeAxisX, closestPack->relativeAxisY, closestPack->relativeAxisZ);
		glm::vec3 collisionZaxis = MathHelper::getVectorDirectionCustomAxis(camera->Right, closestPack->relativeAxisX, closestPack->relativeAxisY, closestPack->relativeAxisZ);

		engine->orientationX = collisionZaxis;
		engine->orientationY = collisionYaxis;
		engine->orientationZ = -collisionXaxis;

		for (int cX = 0; cX <= engine->collisionBox.x; cX++)
		{
			for (int cY = 0; cY <= engine->collisionBox.y; cY++)
			{
				for (int cZ = 0; cZ <= engine->collisionBox.z; cZ++)
				{
					glm::ivec3 position = collisionXaxis * (float)cX + collisionYaxis * (float)cY + collisionZaxis * (float)cZ + (glm::vec3)newBlockRelPos;
					std::vector<std::shared_ptr<Chunk>>::iterator chunkIt = closestPack->chunkList.begin();
					while (clear && chunkIt != closestPack->chunkList.end()) {
						std::vector<std::shared_ptr<Block>>::iterator entIt = (*chunkIt)->blockList.begin();

						while (clear && entIt != (*chunkIt)->blockList.end()) {

							if ((*entIt)->relPosition == position) {
								clear = false;
							}
							entIt++;
						}
						chunkIt++;
					}
				}
			}
		}

		if (clear) {

			glm::vec3 a = (glm::vec3)newBlockRelPos / (float)Configuration::CHUNK_SIZE; //var a only used in the following 4 lines
			a.x = a.x < 0 ? a.x - 1 : a.x;
			a.y = a.y < 0 ? a.y - 1 : a.y; //first chunk in xPos will be 0,0,0 ----- first chunk in xNeg will be -1,0,0
			a.z = a.z < 0 ? a.z - 1 : a.z;
			glm::ivec3 newBlockChunk = (glm::ivec3)a;  //casting to integer will truncate the float values  12.35->12 , -15.247->-15

			std::vector<std::shared_ptr<Chunk>>::iterator chunk = closestPack->chunkList.begin();

			bool found = false;

			while (!found && chunk != closestPack->chunkList.end()) { //iterate through all chunks in the chunk pack to see if the one the block should be on already exists
				if ((*chunk)->relativePosition == newBlockChunk) {
					found = true;
				}
				else {
					chunk++;
				}
			}

			std::shared_ptr<Engine> copyForConnection = engine;

			if (found) {
				engine->relPosition = newBlockRelPos;
				engine->setUpProps();
				(*chunk)->engineList.push_back(std::move(engine));
				(*chunk)->updateChunkVAO();
			}
			else {
				engine->relPosition = closestBlock->relPosition + offset;
				engine->setUpProps();
				std::shared_ptr<Chunk> newChunk(new Chunk(newBlockChunk.x, newBlockChunk.y, newBlockChunk.z));
				newChunk->engineList.push_back(std::move(engine));
				newChunk->updateChunkVAO();

				closestPack->chunkList.push_back(std::move(newChunk));
			}

			std::shared_ptr<HelperClasses::Connection> newConnection(new HelperClasses::Connection());
			newConnection->type = copyForConnection->inputType;
			glm::ivec3 outputSpot = copyForConnection->orientationX * copyForConnection->input.x +
				copyForConnection->orientationY * copyForConnection->input.y +
				copyForConnection->orientationZ * copyForConnection->input.z;

			newConnection->engineList.push_back(std::move(copyForConnection));
			newConnection->wirePoints.push_back(outputSpot);
			closestPack->connectionList.push_back(std::move(newConnection));
		}
	}
}
*/

void EnvironmentManager::buildEntityFromPreview(EntityManager::previewModelData* preview)
{
	switch (entityManager->previewModel.entityProps.SPAWNING_METHOD) {

	case Configuration::SPAWN_METHOD::POLYGON_BOX:
		this->polygonSpawn(preview);

		break;

	case Configuration::SPAWN_METHOD::MODEL:
		this->modelSpawn(preview);

		break;

	case Configuration::SPAWN_METHOD::LINEAR_PATH:
		this->linearSpawn(preview);

		break;

	}

	/*if (newEntity->arrayCreation) {

		float dist = Configuration::MAXIMUM_JOIN_RANGE * 2;//just to make sure the condition will be valid
		glm::ivec3 localClosestPoint;
		Chunk* closestChunk = NULL;
		ChunkPack* closestPack = NULL;
		glm::vec3 closestRelPosition;
		glm::ivec3 closestNewBlockChunk;

		//This iteration finds the closest block to the new object location
		for (std::vector<std::shared_ptr<ChunkPack>>::iterator chunkPack = chunkPacks->begin(); chunkPack != chunkPacks->end(); chunkPack++)
		{
			glm::vec3 relPosition = MathHelper::changeReferenceSystem(startPoint, (*chunkPack)->absPosition, (*chunkPack)->relativeAxisX, (*chunkPack)->relativeAxisY, (*chunkPack)->relativeAxisZ);

			for (std::vector<std::shared_ptr<Chunk>>::iterator chunk = (*chunkPack)->chunkList.begin(); chunk != (*chunkPack)->chunkList.end(); chunk++)
			{
				//if the distance between the center of the chunk and the point is less than the lentgth of the diagonal of the chunk plus the join range of the cube, the block will be in range to join the chunk
				if (MathHelper::getVectorLength((glm::vec3)(*chunk)->getChunkCenter() - relPosition) < sqrt(3 * pow(Configuration::CHUNK_SIZE / 2, 2)) + Configuration::MAXIMUM_JOIN_RANGE) {
					//if we entered this loop is becouse the chunk is close enough to the block creation so that it may contain a block to join the new one
					//iterate through all of the blocks to find the closest one
					for (std::map<glm::ivec3, std::shared_ptr<Entity>>::iterator point = (*chunk)->collisionPoints.begin(); point != (*chunk)->collisionPoints.end(); point++)
					{
						glm::vec3 posDiference = relPosition - (glm::vec3)point->first;
						float newDist = MathHelper::getVectorLength(posDiference);

						if (newDist < Configuration::MAXIMUM_JOIN_RANGE) {

							if (newDist < dist) { //if the block is in join range and closer than the last one
								dist = newDist;
								localClosestPoint = point->first;  //point is an iterator pointer. We need to get the object the iterator is pointing to
								closestChunk = &(*(*chunk)); //chunk is an iterator pointer that aims to a chunk shared pointer
								closestPack = &(*(*chunkPack));
								closestRelPosition = relPosition;
							};
						}
					}
				}
			}
		}

		//std::vector<glm::vec3>

		if (closestChunk == NULL) {



		}
		else {
			//If there is no chunk nearby...
			glm::vec3 blockSize = MathHelper::changeReferenceSystem(endPoint - startPoint, glm::vec3(0, 0, 0), assets->preview.previewRefSystem[0], assets->preview.previewRefSystem[1], assets->preview.previewRefSystem[2]);

			int xWise = (blockSize.x > 0) ? 1 : -1;
			int yWise = (blockSize.y > 0) ? 1 : -1;
			int zWise = (blockSize.z < 0) ? 1 : -1;

			std::vector<glm::ivec3> newBlocks;

			for (int x = 0; std::abs(x) <= std::abs(blockSize.x); x += xWise)
			{
				for (int y = 0; std::abs(y) <= std::abs(blockSize.y); y += yWise)
				{
					for (int z = 0; std::abs(z) <= std::abs(blockSize.z); z += zWise)
					{
						newBlocks.push_back(glm::vec3(x, y, z));
					}
				}
			};

			std::shared_ptr<ChunkPack> newPack(new ChunkPack());

			newPack->absPosition = startPoint;

			newPack->relativeAxisX = assets->preview.previewRefSystem[0];
			newPack->relativeAxisY = assets->preview.previewRefSystem[1];
			newPack->relativeAxisZ = assets->preview.previewRefSystem[2];

			newPack->pitch = assets->preview.previewAngles[0];
			newPack->yaw = assets->preview.previewAngles[1];
			newPack->roll = assets->preview.previewAngles[2];

			chunkPacks->push_back(newPack);

			std::vector<Chunk*> chunksVAOupdate;

			for each (glm::ivec3 newPosition in newBlocks)
			{
				glm::vec3 a = (glm::vec3)newPosition / (float)Configuration::CHUNK_SIZE; //var a only used in the following 4 lines
				a.x = a.x < 0 ? a.x - 1 : a.x;
				a.y = a.y < 0 ? a.y - 1 : a.y; //first chunk in xPos will be 0,0,0 ----- first chunk in xNeg will be -1,0,0
				a.z = a.z < 0 ? a.z - 1 : a.z;
				glm::ivec3 newBlockChunk = (glm::ivec3)a;  //casting to integer will truncate the float values  12.35->12 , -15.247->-15

				Chunk* chunkToAppend = NULL;
				chunkToAppend = newPack->getChunkAt(newBlockChunk);

				std::shared_ptr<Entity> entityToAdd = entityManager->entityFactory(entityType);
				entityToAdd->relPosition = newPosition;
				entityToAdd->readyUp();

				if (chunkToAppend != NULL) {
					chunkToAppend->visualEntities.push_back(entityToAdd);

					chunkToAppend->collisionPoints.insert(std::pair<glm::ivec3, std::shared_ptr<Entity>>(newPosition, entityToAdd));

					chunksVAOupdate.push_back(chunkToAppend);
					chunkToAppend->VAOupdatePending = true;
				}
				else {
					std::shared_ptr<Chunk> newChunk(new Chunk(newBlockChunk));

					newChunk->visualEntities.push_back(entityToAdd);
					newChunk->collisionPoints.insert(std::pair<glm::ivec3, std::shared_ptr<Entity>>(newPosition, entityToAdd));

					newPack->chunkList.push_back(newChunk);
					chunksVAOupdate.push_back(&(*newChunk));
					newChunk->VAOupdatePending = true;

				}
			}

			for each (Chunk* vaoUpdate in chunksVAOupdate)
			{
				if (vaoUpdate->VAOupdatePending) {
					vaoUpdate->updateChunkVAO();
				}
			}
		//}
	}
	else {
		processObjectSpawn(startPoint.x, startPoint.y, startPoint.z, entityType);
	}*/

}

void EnvironmentManager::setNewPreview(EntityManager::ENTITY_LIST entity, glm::vec3 startPosition)
{

	float dist = Configuration::MAXIMUM_JOIN_RANGE * 2; //just to validate the first iteration of the if

	Chunk* closestChunk = NULL;
	ChunkPack* closestPack = NULL;
	glm::vec3 closestRelPosition;
	glm::vec3 shortestDistanceToPol;
	HelperClasses::Polygon * closestPoligon = NULL;

	for (std::vector<std::shared_ptr<ChunkPack>>::iterator chunkPack = chunkPacks->begin(); chunkPack != chunkPacks->end(); chunkPack++)
	{
		glm::vec3 relPosition = MathHelper::changeReferenceSystem(startPosition, (*chunkPack)->absPosition, (*chunkPack)->relativeAxisX, (*chunkPack)->relativeAxisY, (*chunkPack)->relativeAxisZ);

		for (std::vector<std::shared_ptr<Chunk>>::iterator chunk = (*chunkPack)->chunkList.begin(); chunk != (*chunkPack)->chunkList.end(); chunk++)
		{
			//if the distance between the center of the chunk and the point is less than the lentgth of the diagonal of the chunk plus the join range of the cube, the poligon will be in range to join the chunk
			if (MathHelper::getVectorLength((glm::vec3)(*chunk)->getChunkCenter() - relPosition) < sqrt(3 * pow(Configuration::CHUNK_SIZE / 2, 2)) + Configuration::MAXIMUM_JOIN_RANGE) {
				//if we entered this loop is becouse the chunk is close enough to the start point of the poligon, so that the poligon will have to join the chunk
				//iterate through all of the poligons in the chunk to see if there is any close enough
				for each (HelperClasses::Polygon pol in (*chunk)->polygons) {

					glm::vec3 polDiagonal = pol.end - pol.start;

					polDiagonal.x = polDiagonal.x / abs(polDiagonal.x);
					polDiagonal.y = polDiagonal.y / abs(polDiagonal.y);
					polDiagonal.z = polDiagonal.z / abs(polDiagonal.z);

					glm::vec3 polMarginsStart = pol.start - glm::vec3(Configuration::MAXIMUM_JOIN_RANGE * polDiagonal.x, Configuration::MAXIMUM_JOIN_RANGE * polDiagonal.y, Configuration::MAXIMUM_JOIN_RANGE * polDiagonal.z);
					glm::vec3 polMarginsEnd = pol.end + glm::vec3(Configuration::MAXIMUM_JOIN_RANGE * polDiagonal.x, Configuration::MAXIMUM_JOIN_RANGE * polDiagonal.y, Configuration::MAXIMUM_JOIN_RANGE * polDiagonal.z);

					if (MathHelper::isInsideParalelogram(relPosition, polMarginsStart, polMarginsEnd)) {
						glm::vec3 shortestVec = MathHelper::findVectorPointToParalelogram(relPosition, pol.start, pol.end);
						if (shortestVec.length() < dist) {
							dist = shortestVec.length();
							closestChunk = &(*(*chunk));
							closestPack = &(*(*chunkPack));
							closestRelPosition = relPosition;
							shortestDistanceToPol = shortestVec;
							closestPoligon = &pol;
						}
					}
				}
			}
		}
	};

	if (closestPoligon == NULL) {
		entityManager->previewModel.startPoint = camera->SpawnPosition;

		entityManager->previewModel.previewRefSystem[0] = -camera->Front;
		entityManager->previewModel.previewRefSystem[1] = camera->Up;
		entityManager->previewModel.previewRefSystem[2] = camera->Right;

		entityManager->previewModel.entityProps = entityManager->getEntityPropsFor(entity);
		entityManager->previewModel.entityName = entity;

		entityManager->previewModel.arrayConstructionCanceled = false;

		entityManager->previewModel.appendToPack = NULL;

		entityManager->previewModel.renderPreview = true;
		entityManager->previewModel.updateVAO();
	}
	else {

		glm::vec3 gridOffset;
		gridOffset.x = (shortestDistanceToPol.x == 0) ? 0 : Configuration::GRID_SIZE * shortestDistanceToPol.x / abs(shortestDistanceToPol.x);
		gridOffset.y = (shortestDistanceToPol.y == 0) ? 0 : Configuration::GRID_SIZE * shortestDistanceToPol.y / abs(shortestDistanceToPol.y);
		gridOffset.z = (shortestDistanceToPol.z == 0) ? 0 : Configuration::GRID_SIZE * shortestDistanceToPol.z / abs(shortestDistanceToPol.z);
		
		glm::vec3 pointToSnap = closestRelPosition;// -shortestDistanceToPol + gridOffset;

		glm::vec3 snappedPoint = this->snapPoint(pointToSnap, Configuration::GRID_SIZE);

		glm::vec3 wtf = closestPack->absPosition + closestRelPosition.x * closestPack->relativeAxisX + closestRelPosition.y * closestPack->relativeAxisZ + closestRelPosition.z * closestPack->relativeAxisZ;

		entityManager->previewModel.startPoint = wtf;

		entityManager->previewModel.previewRefSystem[0] = closestPack->relativeAxisX;
		entityManager->previewModel.previewRefSystem[1] = closestPack->relativeAxisY;
		entityManager->previewModel.previewRefSystem[2] = closestPack->relativeAxisZ;

		entityManager->previewModel.entityProps = entityManager->getEntityPropsFor(entity);
		entityManager->previewModel.entityName = entity;

		entityManager->previewModel.arrayConstructionCanceled = false;

		entityManager->previewModel.renderPreview = true;
		entityManager->previewModel.appendToPack = closestPack;
		entityManager->previewModel.updateVAO();
	}

}

void EnvironmentManager::updatePreview()
{
	switch (entityManager->previewModel.entityProps.SPAWNING_METHOD) {
	case Configuration::SPAWN_METHOD::MODEL:
		entityManager->previewModel.startPoint = camera->SpawnPosition;
		break;

	case Configuration::SPAWN_METHOD::LINEAR_PATH:
		entityManager->previewModel.endPoint = camera->SpawnPosition;
		break;

	case Configuration::SPAWN_METHOD::POLYGON_BOX:

		if (entityManager->previewModel.appendToPack == NULL) {
			glm::vec3 relSnap = MathHelper::changeReferenceSystem(camera->SpawnPosition, entityManager->previewModel.startPoint, entityManager->previewModel.previewRefSystem[0], entityManager->previewModel.previewRefSystem[1], entityManager->previewModel.previewRefSystem[2]);

			glm::vec3 snappedPoint = this->snapPoint(relSnap, 0.25f);

			glm::vec3 finalPoint = entityManager->previewModel.startPoint + entityManager->previewModel.previewRefSystem[0] * snappedPoint.x + entityManager->previewModel.previewRefSystem[1] * snappedPoint.y + entityManager->previewModel.previewRefSystem[2] * snappedPoint.z;

			entityManager->previewModel.endPoint = finalPoint;

			entityManager->previewModel.updateVAO();
		}
		else {
			glm::vec3 relSnap = MathHelper::changeReferenceSystem(camera->SpawnPosition, entityManager->previewModel.startPoint, entityManager->previewModel.previewRefSystem[0], entityManager->previewModel.previewRefSystem[1], entityManager->previewModel.previewRefSystem[2]);

			glm::vec3 snappedPoint = this->snapPoint(relSnap, 0.25f);

			glm::vec3 finalPoint = entityManager->previewModel.startPoint + entityManager->previewModel.previewRefSystem[0] * snappedPoint.x + entityManager->previewModel.previewRefSystem[1] * snappedPoint.y + entityManager->previewModel.previewRefSystem[2] * snappedPoint.z;

			entityManager->previewModel.endPoint = finalPoint;

			entityManager->previewModel.updateVAO();
		}

		

		break;
	}
}

void EnvironmentManager::processObjectSpawn(float x, float y, float z, EntityManager::ENTITY_LIST entityType)
{
	/* To know if there is an existent chunk in witch the object should be appended we will find the position of the new block in each chunk's relative coordinate system
	*  With that position we will iterate through all existent blocks in chunks of every chunkPack to find the closest one.
	*  Then, if there is a block close enough, the new object will be appended to it, creating a new chunk if needed.
	*  Otherwise, a new chunk pack will be created.
	*/

	float dist = Configuration::MAXIMUM_JOIN_RANGE * 2;//just to make sure the condition will be valid
	glm::ivec3 closestPoint;
	Chunk* closestChunk = NULL;
	ChunkPack* closestPack = NULL;
	glm::vec3 closestRelPosition;
	glm::ivec3 closestNewBlockChunk;

	//This iteration finds the closest block to the new object location
	for (std::vector<std::shared_ptr<ChunkPack>>::iterator chunkPack = chunkPacks->begin(); chunkPack != chunkPacks->end(); chunkPack++)
	{
		glm::vec3 relPosition = MathHelper::changeReferenceSystem(glm::vec3(x, y, z), (*chunkPack)->absPosition, (*chunkPack)->relativeAxisX, (*chunkPack)->relativeAxisY, (*chunkPack)->relativeAxisZ);

		for (std::vector<std::shared_ptr<Chunk>>::iterator chunk = (*chunkPack)->chunkList.begin(); chunk != (*chunkPack)->chunkList.end(); chunk++)
		{
			//if the distance between the center of the chunk and the point is less than the lentgth of the diagonal of the chunk plus the join range of the cube, the block will be in range to join the chunk
			if (MathHelper::getVectorLength((glm::vec3)(*chunk)->getChunkCenter() - relPosition) < sqrt(3 * pow(Configuration::CHUNK_SIZE / 2, 2)) + Configuration::MAXIMUM_JOIN_RANGE) {
				//if we entered this loop is becouse the chunk is close enough to the block creation so that it may contain a block to join the new one
				//iterate through all of the blocks to find the closest one
				for (std::map<glm::ivec3, std::shared_ptr<Entity>>::iterator point = (*chunk)->collisionPoints.begin(); point != (*chunk)->collisionPoints.end(); point++)
				{
					glm::vec3 posDiference = relPosition - (glm::vec3)point->first;
					float newDist = MathHelper::getVectorLength(posDiference);

					if (newDist < Configuration::MAXIMUM_JOIN_RANGE) {

						if (newDist < dist) { //if the block is in join range and closer than the last one
							dist = newDist;
							closestPoint = point->first;  //point is an iterator pointer. We need to get the object the iterator is pointing to 
							closestChunk = &(*(*chunk)); //chunk is an iterator pointer that aims to a chunk shared pointer
							closestPack = &(*(*chunkPack));
							closestRelPosition = relPosition;
						};
					}
				}

				/*for (std::vector<std::shared_ptr<WireHolder>>::iterator wh = (*chunk)->wireHolderList.begin(); wh != (*chunk)->wireHolderList.end(); wh++)
				{
					glm::vec3 posDiference = relPosition - (glm::vec3)(*wh)->position;
					float newDist = MathHelper::getVectorLength(posDiference);

					if (newDist < Configuration::MAXIMUM_JOIN_RANGE) {

						if (newDist < dist) { //if the block is in join range and closer than the last one
							dist = newDist;
							closestWireHolder = &(*(*wh));  //point is an iterator pointer. We need to get the object the iterator is pointing to
							closestChunk = &(*(*chunk));
							closestPack = &(*(*chunkPack));
							closestRelPosition = relPosition;
						};
					}
				}*/
			}
		}
	}

	std::shared_ptr<Entity> newEntity = entityManager->entityFactory(entityType);

	std::map<glm::ivec3, std::shared_ptr<Entity>> newColisionPoints;
	ChunkPack* addConnectionsToPack = NULL;

	if (closestPack != NULL) {

		glm::vec3 closestAbsPoint = (glm::vec3)closestPoint + closestPack->absPosition;

		glm::ivec3 offset = MathHelper::getVectorDirection(closestRelPosition - (glm::vec3)closestPoint);

		glm::ivec3 newBlockRelPos = closestPoint + offset;

		//checking colision box
		bool clear = true;

		//snapping camera axis to chunk's axis
		glm::ivec3 collisionXaxis = MathHelper::getVectorDirectionCustomAxis(camera->Front, closestPack->relativeAxisX, closestPack->relativeAxisY, closestPack->relativeAxisZ);
		glm::ivec3 collisionYaxis = MathHelper::getVectorDirectionCustomAxis(camera->Up, closestPack->relativeAxisX, closestPack->relativeAxisY, closestPack->relativeAxisZ);
		glm::ivec3 collisionZaxis = MathHelper::getVectorDirectionCustomAxis(camera->Right, closestPack->relativeAxisX, closestPack->relativeAxisY, closestPack->relativeAxisZ);

		newEntity->orientationX = glm::ivec3(1, 0, 0);
		newEntity->orientationY = glm::ivec3(0, 1, 0);
		newEntity->orientationZ = glm::ivec3(0, 0, 1);

		for (int cX = 0; cX <= newEntity->collisionBox.x - 1; cX++)
		{
			for (int cY = 0; cY <= newEntity->collisionBox.y - 1; cY++)
			{
				for (int cZ = 0; cZ <= newEntity->collisionBox.z - 1; cZ++)
				{
					glm::ivec3 position = collisionXaxis * cX + collisionYaxis * cY + collisionZaxis * cZ + newBlockRelPos;

					newColisionPoints.insert(std::pair<glm::ivec3, std::shared_ptr<Entity>>(position, newEntity));

					std::vector<std::shared_ptr<Chunk>>::iterator chunkIt = closestPack->chunkList.begin();
					while (clear && chunkIt != closestPack->chunkList.end()) {

						std::map<glm::ivec3, std::shared_ptr<Entity>>::iterator mapIt = (*chunkIt)->collisionPoints.begin();

						while (clear && mapIt != (*chunkIt)->collisionPoints.end()) {

							if (mapIt->first == position) {
								clear = false;
								std::cout << "Colision!: " << position.x << ", " << position.y << ", " << position.z << "  -----  " << mapIt->first.x << ", " << mapIt->first.y << ", " << mapIt->first.z << std::endl;
							}
							else {
								mapIt++;
							}
						}
						chunkIt++;
					}
				}
			}
		}

		if (clear) {

			glm::vec3 a = (glm::vec3)newBlockRelPos / (float)Configuration::CHUNK_SIZE; //var a only used in the following 4 lines
			a.x = a.x < 0 ? a.x - 1 : a.x;
			a.y = a.y < 0 ? a.y - 1 : a.y; //first chunk in xPos will be 0,0,0 ----- first chunk in xNeg will be -1,0,0
			a.z = a.z < 0 ? a.z - 1 : a.z;
			glm::ivec3 newBlockChunk = (glm::ivec3)a;  //casting to integer will truncate the float values  12.35->12 , -15.247->-15

			Chunk* chunkToSpawn = NULL;
			if (newBlockChunk == closestChunk->relativePosition) { //it's likely that the chunk in witch the new entity has to spawn is the same chunk of the closest entitiy we found earlier.
				chunkToSpawn = closestChunk;
			}
			else { // If not, we will iterate looking for it
				chunkToSpawn = closestPack->getChunkAt(newBlockChunk);
			}

			if (chunkToSpawn == NULL) {
				newEntity->relPosition = newBlockRelPos;
				newEntity->readyUp();

				std::shared_ptr<Chunk> newChunk(new Chunk(newBlockChunk.x, newBlockChunk.y, newBlockChunk.z));
				newChunk->visualEntities.push_back(newEntity);
				newChunk->updateChunkVAO();

				closestPack->chunkList.push_back(std::move(newChunk));
			}
			else {
				newEntity->relPosition = newBlockRelPos;
				newEntity->readyUp();

				chunkToSpawn->visualEntities.push_back(newEntity);
				chunkToSpawn->updateChunkVAO();
			}

			closestPack->addColisionPoints(newColisionPoints);
			addConnectionsToPack = closestPack;
		}
	}
	else { //if there is no chunk close enough a new chunkPack will be created

		std::shared_ptr<ChunkPack> newPack(new ChunkPack());

		newPack->absPosition = glm::vec3(x, y, z);

		newPack->relativeAxisX = camera->Right;
		newPack->relativeAxisY = camera->Up;
		newPack->relativeAxisZ = -camera->Front;

		newPack->pitch = camera->Pitch;
		newPack->yaw = camera->Yaw;
		newPack->roll = camera->Roll;

		addConnectionsToPack = &(*newPack);

		std::shared_ptr<Chunk> newChunk(new Chunk(0, 0, 0));

		newEntity->relPosition = glm::vec3(0, 0, 0);
		newEntity->readyUp();

		newChunk->visualEntities.push_back(newEntity);
		newChunk->updateChunkVAO();

		newPack->chunkList.push_back(newChunk);
		chunkPacks->push_back(newPack);

		for (int cX = 0; cX <= newEntity->collisionBox.x - 1; cX++)
		{
			for (int cY = 0; cY <= newEntity->collisionBox.y - 1; cY++)
			{
				for (int cZ = 0; cZ <= newEntity->collisionBox.z - 1; cZ++)
				{
					glm::ivec3 position = glm::ivec3(cX, cY, cZ) + newEntity->relPosition;

					newColisionPoints.insert(std::pair<glm::ivec3, std::shared_ptr<Entity>>(position, newEntity));
				}
			}
		}

		newPack->addColisionPoints(newColisionPoints);
	}

	if (addConnectionsToPack != NULL) {

		std::vector<std::shared_ptr<Wire>> newWires;

		for each (std::pair<Configuration::RESOURCE_TYPE, glm::ivec3> inputs in newEntity->inputPoints)
		{
			std::shared_ptr<Wire> newWire(new Wire());
		}

		for each (std::pair<Configuration::RESOURCE_TYPE, glm::ivec3> inputs in newEntity->inputPoints)
		{}

		for each (std::pair<Configuration::RESOURCE_TYPE, glm::ivec3> inputs in newEntity->inputPoints)
		{
			bool done = false;
			std::vector<std::shared_ptr<HelperClasses::Connection>>::iterator conn = addConnectionsToPack->connectionList.begin();

			while (!done && conn != addConnectionsToPack->connectionList.end()) {

				if ((*conn)->type == inputs.first) {


					done = true;
				}
				else {
					conn++;
				}

			}

			if (!done) {

			}
		}

	}

}

void EnvironmentManager::polygonSpawn(EntityManager::previewModelData *preview)
{
	if (preview->appendToPack == NULL) {
		HelperClasses::Polygon newPolygon;

		std::shared_ptr<ChunkPack> newPack(new ChunkPack());

		newPack->absPosition = preview->startPoint;

		newPack->absPosition = preview->startPoint;
		newPack->relativeAxisX = preview->previewRefSystem[0];
		newPack->relativeAxisY = preview->previewRefSystem[1];
		newPack->relativeAxisZ = preview->previewRefSystem[2];

		std::vector<Chunk*> chunksVAOupdate;

		glm::vec3 farVertex = MathHelper::changeReferenceSystem(preview->endPoint, preview->startPoint, preview->previewRefSystem[0], preview->previewRefSystem[1], preview->previewRefSystem[2]);
		glm::vec3 nearVertex = glm::vec3(0, 0, 0);
		glm::vec3 textureCoord = glm::abs(farVertex);

		glm::vec3 growingSide(farVertex.x / abs(farVertex.x), farVertex.y / abs(farVertex.y), farVertex.z / abs(farVertex.z));

		glm::vec3 newPolNearVertex = nearVertex;
		glm::vec3 newPolFarVertex(0, 0, 0);

		bool emergencyEVAC = false;
		int counter = 0;

		while (abs(newPolFarVertex.x) < abs(farVertex.x) && !emergencyEVAC) {

			newPolFarVertex.x = (abs(newPolNearVertex.x) + Configuration::CHUNK_SIZE <= abs(farVertex.x)) ? newPolNearVertex.x + Configuration::CHUNK_SIZE * growingSide.x : farVertex.x;
			newPolNearVertex.y = 0;
			newPolFarVertex.y = 0;

			while (abs(newPolFarVertex.y) < abs(farVertex.y) && !emergencyEVAC) {

				newPolFarVertex.y = (abs(newPolNearVertex.y) + Configuration::CHUNK_SIZE <= abs(farVertex.y)) ? newPolNearVertex.y + Configuration::CHUNK_SIZE * growingSide.y : farVertex.y;
				newPolNearVertex.z = 0;
				newPolFarVertex.z = 0;

				while (abs(newPolFarVertex.z) < abs(farVertex.z) && !emergencyEVAC) {

					newPolFarVertex.z = (abs(newPolNearVertex.z) + Configuration::CHUNK_SIZE <= abs(farVertex.z)) ? newPolNearVertex.z + Configuration::CHUNK_SIZE * growingSide.z : farVertex.z;

					glm::vec3 a = (glm::vec3)newPolNearVertex / (float)Configuration::CHUNK_SIZE; //var a only used in the following 4 lines
					a.x = a.x < 0 ? a.x - 1 : a.x;
					a.y = a.y < 0 ? a.y - 1 : a.y; //first chunk in xPos will be 0,0,0 ----- first chunk in xNeg will be -1,0,0
					a.z = a.z < 0 ? a.z - 1 : a.z;
					glm::ivec3 newBlockChunk = (glm::ivec3)a;  //casting to integer will truncate the float values  12.35->12 , -15.247->-15

					std::shared_ptr<Chunk> newChunk(new Chunk(newBlockChunk));

					newPolygon.vertexArray = HelperClasses::getPolyconVertices(newPolNearVertex, newPolFarVertex, newPolFarVertex - newPolNearVertex);
					newPolygon.start = newPolNearVertex;
					newPolygon.end = newPolFarVertex;

					newChunk->polygons.push_back(newPolygon);
					newChunk->updateChunkVAO();
					newPack->chunkList.push_back(std::move(newChunk));

					counter++;
					if (counter > 30000) { emergencyEVAC = true; std::cout << "EMERGENCY EVAC FROM SPAWNING WHILE" << std::endl; }

					newPolNearVertex.z = newPolFarVertex.z;
				}
				newPolNearVertex.y = newPolFarVertex.y;
			}
			newPolNearVertex.x = newPolFarVertex.x;
		}

		chunkPacks->push_back(std::move(newPack));
		
	}
	else {

		HelperClasses::Polygon newPolygon;

		std::vector<Chunk*> chunksVAOupdate;

		glm::vec3 relPosition = preview->startPoint;

		glm::vec3 farVertex = preview->endPoint;
		glm::vec3 nearVertex = preview->startPoint;
		glm::vec3 diagonal = farVertex - nearVertex;

		glm::vec3 growingSide(diagonal.x / abs(diagonal.x), diagonal.y / abs(diagonal.y), diagonal.z / abs(diagonal.z));
		
		glm::vec3 newPolNearVertex = nearVertex;
		glm::vec3 newPolFarVertex(0, 0, 0);

		bool emergencyEVAC = false;
		int counter = 0;

		while (abs(newPolFarVertex.x) < abs(farVertex.x) && !emergencyEVAC) {

			newPolFarVertex.x = (abs(newPolNearVertex.x) + Configuration::CHUNK_SIZE <= abs(farVertex.x)) ? newPolNearVertex.x + Configuration::CHUNK_SIZE * growingSide.x : farVertex.x;
			newPolNearVertex.y = 0;
			newPolFarVertex.y = 0;

			while (abs(newPolFarVertex.y) < abs(farVertex.y) && !emergencyEVAC) {

				newPolFarVertex.y = (abs(newPolNearVertex.y) + Configuration::CHUNK_SIZE <= abs(farVertex.y)) ? newPolNearVertex.y + Configuration::CHUNK_SIZE * growingSide.y : farVertex.y;
				newPolNearVertex.z = 0;
				newPolFarVertex.z = 0;

				while (abs(newPolFarVertex.z) < abs(farVertex.z) && !emergencyEVAC) {

					newPolFarVertex.z = (abs(newPolNearVertex.z) + Configuration::CHUNK_SIZE <= abs(farVertex.z)) ? newPolNearVertex.z + Configuration::CHUNK_SIZE * growingSide.z : farVertex.z;

					glm::vec3 a = (glm::vec3)newPolNearVertex / (float)Configuration::CHUNK_SIZE; //var a only used in the following 4 lines
					a.x = a.x < 0 ? a.x - 1 : a.x;
					a.y = a.y < 0 ? a.y - 1 : a.y; //first chunk in xPos will be 0,0,0 ----- first chunk in xNeg will be -1,0,0
					a.z = a.z < 0 ? a.z - 1 : a.z;
					glm::ivec3 newBlockChunk = (glm::ivec3)a;  //casting to integer will truncate the float values  12.35->12 , -15.247->-15

					std::shared_ptr<Chunk> newChunk(new Chunk(newBlockChunk));

					newPolygon.vertexArray = HelperClasses::getPolyconVertices(newPolNearVertex, newPolFarVertex, newPolFarVertex - newPolNearVertex);
					newPolygon.start = newPolNearVertex;
					newPolygon.end = newPolFarVertex;

					newChunk->polygons.push_back(newPolygon);
					newChunk->updateChunkVAO();
					preview->appendToPack->chunkList.push_back(std::move(newChunk));

					counter++;
					if (counter > 30000) { emergencyEVAC = true; std::cout << "EMERGENCY EVAC FROM SPAWNING WHILE" << std::endl; }

					newPolNearVertex.z = newPolFarVertex.z;
				}
				newPolNearVertex.y = newPolFarVertex.y;
			}
			newPolNearVertex.x = newPolFarVertex.x;
		}

		for each (Chunk* vaoUpdates in chunksVAOupdate)
		{
			vaoUpdates->updateChunkVAO();
		}

	}

	preview->renderPreview = false;
}

void EnvironmentManager::modelSpawn(EntityManager::previewModelData *preview)
{
}

void EnvironmentManager::linearSpawn(EntityManager::previewModelData* preview)
{
}

glm::vec3 EnvironmentManager::snapPoint(glm::vec3 pointToSnap, float gridSize)
{
	float divX = pointToSnap.x / gridSize;
	float divY = pointToSnap.y / gridSize;
	float divZ = pointToSnap.z / gridSize;

	divX = (abs(divX) < 1) ? divX / abs(divX) : round(divX); //if div is less than 1 it may be rounded to 0. To prevent that, it will be set to 1 or -1 automatically
	divY = (abs(divY) < 1) ? divY / abs(divY) : round(divY);
	divZ = (abs(divZ) < 1) ? divZ / abs(divZ) : round(divZ);

	glm::vec3 snappedPoint(divX * gridSize, divY * gridSize, divZ * gridSize);

	return snappedPoint;
}
