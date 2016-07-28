#include "EnvironmentManager.h"

extern Renderer renderer;
extern std::vector<ChunkPack> chunkPacks;
extern EntityContainer entityContainer;
extern glm::vec3 closestPoint; //This is the last closest point found, used in the renderer class to debug

extern Camera camera;

EnvironmentManager::EnvironmentManager()
{
}

EnvironmentManager::~EnvironmentManager()
{
}

void EnvironmentManager::processObjectSpawn(float x, float y, float z, EntityContainer::ENTITY_LIST entity)
{
	/* To know if there is an existent chunk in witch the object should be appended we will find the position of the new block in each chunk's relative coordinate system
	*  With that position we will iterate through all existent blocks in chunks of every chunkPack to find the closest one.
	*  Then, if there is a block close enough, the new object will be appended to it, creating a new chunk if needed.
	*  Otherwise, a new chunk pack will be created.
	*/

	float dist = Configuration::MAXIMUM_JOIN_RANGE * 2;//just to make sure the condition will be valid
	Entity* closestBlock = NULL;
	Chunk* closestChunk = NULL;
	ChunkPack* closestPack = NULL;
	glm::vec3 closestRelPosition;
	glm::ivec3 closestNewBlockChunk;

	//This iteration finds the closest block to the new object location
	for (std::vector<ChunkPack>::iterator chunkPack = chunkPacks.begin(); chunkPack != chunkPacks.end(); chunkPack++)
	{
		glm::vec3 relPosition = MathHelper::changeReferenceSystem(glm::vec3(x, y, z), chunkPack->absPosition, chunkPack->relativeAxisX, chunkPack->relativeAxisY, chunkPack->relativeAxisZ);

		for (std::vector<Chunk>::iterator chunk = chunkPack->chunkList.begin(); chunk != chunkPack->chunkList.end(); chunk++)
		{
			//if the distance between the center of the chunk and the point is less than the lentgth of the diagonal of the chunk plus the join range of the cube, the block will be in range to join the chunk
			if (MathHelper::getVectorLength((glm::vec3)chunk->getChunkCenter() - relPosition) < sqrt(3 * pow(Configuration::CHUNK_SIZE / 2, 2)) + Configuration::MAXIMUM_JOIN_RANGE) {
				//if we entered this loop is becouse the chunk is close enough to the block creation so that it may contain a block to join the new one
				//iterate through all of the blocks to find the closest one
				for (std::vector<Block>::iterator obj = chunk->blockList.begin(); obj != chunk->blockList.end(); obj++)
				{
					glm::vec3 posDiference = relPosition - (glm::vec3)obj->relPosition;
					float newDist = MathHelper::getVectorLength(posDiference);

					if (newDist < Configuration::MAXIMUM_JOIN_RANGE) {

						if (newDist < dist) { //if the block is in join range and closer than the last one
							dist = newDist;
							closestBlock = &(*obj);  //obj is an iterator pointer. We need to get the object the iterator is pointing to and then get the -object- pointer
							closestChunk = &(*chunk);
							closestPack = &(*chunkPack);
							closestRelPosition = relPosition;
						};
					}
				}
			}
		}
	}

	if (closestPack != NULL) {
		closestPoint = (glm::vec3)closestBlock->relPosition + closestPack->absPosition;
	}

	//auto obj = entities.find(id)->second;
	switch (entity)
	{
		case EntityContainer::STARTENUM:
			break;
		case EntityContainer::RUSTED_BLOCK:
		{
			spawnCube(glm::vec3(x, y, z), closestRelPosition, closestBlock, closestChunk, closestPack, entityContainer.rustedBlock);
			break;
		}

		case EntityContainer::FLOOR_BLOCK:
		{
			spawnCube(glm::vec3(x, y, z), closestRelPosition, closestBlock, closestChunk, closestPack, entityContainer.floorBlock);
			break;
		}

		case EntityContainer::WALL_BLOCK:
		{
			spawnCube(glm::vec3(x, y, z), closestRelPosition, closestBlock, closestChunk, closestPack, entityContainer.wallBlock);
			break;
		}

		case EntityContainer::GENERATOR:
		{
			spawnGenerator(glm::vec3(x, y, z), closestRelPosition, closestBlock, closestChunk, closestPack, entityContainer.generator);
			break;
		}

		case EntityContainer::WIRE:
		{
			spawnWire(glm::vec3(x, y, z), closestRelPosition, closestBlock, closestChunk, closestPack, entityContainer.wire);
			break;
		}

		case EntityContainer::ENDENUM:
			break;
		default:
			break;
	}

	/*
	switch (entities.find(id)->second.type) {

		case Configuration::ENTITIES_TYPES::block:
		{
			//Block& obj = dynamic_cast<Block&>(entities.find(id)->second);
			spawnCube(glm::vec3(x, y, z), closestRelPosition, closestBlock, closestChunk, closestPack, obj);
			break;
		}

		case Configuration::ENTITIES_TYPES::generator:
		{
			Generator& obj = dynamic_cast<Generator&>(entities.find(id)->second);
			spawnGenerator(glm::vec3(x, y, z), closestRelPosition, closestBlock, closestChunk, closestPack, obj);
			break;
		}

	}*/
}

void EnvironmentManager::deleteModel(float x, float y, float z) {

	/* To delete a block we will change the reference system of the point for each chunk.
	*  Then, we will look for the closes block to that point in the pack
	*  If there is a block, it will be deleted.
	*/

	float dist = Configuration::MAXIMUM_JOIN_RANGE * 2;//just to make sure the condition will be valid
	std::vector<Block>::iterator closestBlock;
	std::vector<Chunk>::iterator closestChunk;
	std::vector<ChunkPack>::iterator closestPack;

	bool found = false;

	//This iteration finds the closest block to the new object location
	for (std::vector<ChunkPack>::iterator chunkPack = chunkPacks.begin(); chunkPack != chunkPacks.end(); chunkPack++)
	{
		glm::vec3 relPosition = MathHelper::changeReferenceSystem(glm::vec3(x, y, z), chunkPack->absPosition, chunkPack->relativeAxisX, chunkPack->relativeAxisY, chunkPack->relativeAxisZ);

		for (std::vector<Chunk>::iterator chunk = chunkPack->chunkList.begin(); chunk != chunkPack->chunkList.end(); chunk++)
		{
			//if the distance between the center of the chunk and the point is less than the lentgth of the diagonal of the chunk plus the join range of the cube, the block will be in range to join the chunk
			if (MathHelper::getVectorLength((glm::vec3)chunk->getChunkCenter() - relPosition) < sqrt(3 * pow(Configuration::CHUNK_SIZE / 2, 2)) + Configuration::MAXIMUM_JOIN_RANGE) {
				//if we entered this loop is becouse the chunk is close enough to the block creation so that it may contain a block to join the new one
				//iterate through all of the blocks to find the closest one
				for (std::vector<Block>::iterator obj = chunk->blockList.begin(); obj != chunk->blockList.end(); obj++)
				{
					glm::vec3 posDiference = relPosition - (glm::vec3)obj->relPosition;
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
		closestChunk->blockList.erase(closestBlock);

		if (closestChunk->blockList.size() == 0) { //if it was the only block in the chunk, the chunk will be deleted

			closestPack->chunkList.erase(closestChunk);

			if (closestPack->chunkList.size() == 0) { //if the chunk was the last of it's pack, the pack will be deleted
				chunkPacks.erase(closestPack);
			}
		}
		else {
			closestChunk->updateChunkVAO(); //otherwise, the chunk's VAO will be updated
		}
	}
}

void EnvironmentManager::loadStar(GlobalLight star)
{
	glUniform3fv(glGetUniformLocation(renderer.defaultShader.Program, "starLightPos"), 1, &star.absPosition[0]);
	glUniform3fv(glGetUniformLocation(renderer.defaultShader.Program, "starLightColor"), 1, &star.color[0]);

	glUniformMatrix4fv(glGetUniformLocation(renderer.defaultShader.Program, "starLightSpaceMatrix"), 1, GL_FALSE, glm::value_ptr(star.lightSpaceMatrix));
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, star.depthMap);
}

void EnvironmentManager::updateDepthMap(GlobalLight * star)
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
	renderer.ortogonalDepthShader.Use();
	glUniformMatrix4fv(glGetUniformLocation(renderer.ortogonalDepthShader.Program, "lightSpaceMatrix"), 1, GL_FALSE, glm::value_ptr(lSM));
	glViewport(0, 0, Configuration::SHADOW_WIDTH, Configuration::SHADOW_HEIGHT);
	glBindFramebuffer(GL_FRAMEBUFFER, star->depthMapFBO);
	glClear(GL_DEPTH_BUFFER_BIT);
	renderer.renderScene(renderer.ortogonalDepthShader);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	star->lightSpaceMatrix = lSM;
	glCullFace(GL_BACK);
}

void EnvironmentManager::loadPointLight(PointLight light)
{
	glUniform3fv(glGetUniformLocation(renderer.defaultShader.Program, "pointLightPos"), 1, &light.absPosition[0]);
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_CUBE_MAP, light.depthCubeMap);
}

void EnvironmentManager::loadPointLightList(PointLight lights[5])
{
	for (int i = 0; i < 5; i++)
	{
		glUniform3fv(glGetUniformLocation(renderer.defaultShader.Program, ("pointLightPos[" + std::to_string(i) + "]").c_str()), 1, &lights[i].absPosition[0]);
		glUniform3fv(glGetUniformLocation(renderer.defaultShader.Program, ("pointLightColor[" + std::to_string(i) + "]").c_str()), 1, &lights[i].color[0]);
		glActiveTexture(GL_TEXTURE2 + i);
		glBindTexture(GL_TEXTURE_CUBE_MAP, lights[i].depthCubeMap);
	}

}

void EnvironmentManager::updateDepthCubeMap(PointLight * light)
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
	renderer.pointDepthShader.Use();
	for (GLuint i = 0; i < 6; ++i)
		glUniformMatrix4fv(glGetUniformLocation(renderer.pointDepthShader.Program, ("shadowTransforms[" + std::to_string(i) + "]").c_str()), 1, GL_FALSE, glm::value_ptr(shadowTransforms[i]));
	glUniform1f(glGetUniformLocation(renderer.pointDepthShader.Program, "far_plane"), far);
	glUniform3fv(glGetUniformLocation(renderer.pointDepthShader.Program, "lightPos"), 1, &light->absPosition[0]);
	renderer.renderScene(renderer.pointDepthShader);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

/*void EnvironmentManager::moveEntities() {
	glm::vec3 weThereYet;
	glm::vec3 relPosition;
	glm::vec3 move;
	glm::vec3 newPos;

	for (int i = 0; i < movingBlocks.size(); i++)
	{
		if (movingBlocks[i]->speed != 0.0f) {
			move = MathHelper::getEntityMovement(movingBlocks[i]);
			movingBlocks[i]->move(move);

			weThereYet = movingBlocks[i]->absPosition - movingBlocks[i]->destination;

			if (MathHelper::getVectorLength(weThereYet) < 0.1f) {
				movingBlocks[i]->absPosition = movingBlocks[i]->destination;
				movingBlocks[i]->speed = 0.0f;
				blocksToRemove.push_back(i);
			}
		}
	}
	int c = 0;
	for (int i = 0; i < blocksToRemove.size(); i++) {
		movingBlocks.erase(movingBlocks.begin() + i - c);
		c--;
	}
	blocksToRemove.clear();
}*/

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

/// <summary>
/// Spawns a cube
/// </summary>
/// <param name="absPosition">The absolute position of the cube, needed if we have to spawn a new chunkPack.</param>
/// <param name="relPosition">The relative position: if the cube has to join another chunkpack.</param>
/// <param name="closestBlock">The closest block.</param>
/// <param name="closestChunk">The closest chunk.</param>
/// <param name="closestPack">The closest pack.</param>
/// <param name="id">The identifier of the cube.</param>
void EnvironmentManager::spawnCube(glm::vec3 absPosition, glm::vec3 relPosition, Entity* closestBlock, Chunk* closestChunk, ChunkPack* closestPack, Block obj)
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

		std::vector<Chunk>::iterator chunk = closestPack->chunkList.begin();

		bool found = false;

		while (!found && chunk != closestPack->chunkList.end()) { //iterate through all chunks in the chunk pack to see if the one the block should be on already exists
			if (chunk->relativePosition == newBlockChunk) {
				found = true;
			}
			else {
				chunk++;
			}
		}

		if (found) {
			obj.relPosition = newBlockRelPos;
			chunk->blockList.push_back(obj);
			chunk->updateChunkVAO();
		}
		else {

			obj.relPosition = closestBlock->relPosition + offset;

			Chunk newChunk(newBlockChunk.x, newBlockChunk.y, newBlockChunk.z);
			newChunk.blockList.push_back(obj);
			newChunk.updateChunkVAO();

			closestPack->chunkList.push_back(newChunk);
		}
	}
	else {//there is no chunk close enough so a new chunk pack will be created
		ChunkPack newPack;

		newPack.relativeAxisX = camera.Right;
		newPack.relativeAxisY = camera.Up;
		newPack.relativeAxisZ = -camera.Front;
		newPack.pitch = camera.Pitch;
		newPack.yaw = camera.Yaw;
		newPack.roll = camera.Roll;

		Chunk newChunk(0, 0, 0);;

		newPack.absPosition = glm::vec3(absPosition);
		obj.relPosition = glm::vec3(0, 0, 0);

		newChunk.blockList.push_back(obj);
		newChunk.updateChunkVAO();

		newPack.chunkList.push_back(newChunk);
		chunkPacks.push_back(newPack);
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
/// <param name="obj">The object "generator" to spawn.</param>
void EnvironmentManager::spawnGenerator(glm::vec3 absPosition, glm::vec3 relPosition, Entity* closestBlock, Chunk* closestChunk, ChunkPack* closestPack, Generator obj) {

	if (closestBlock != NULL) {

		glm::ivec3 offset = MathHelper::getVectorDirection((glm::vec3)relPosition - glm::vec3(closestBlock->relPosition));

		glm::ivec3 newBlockRelPos = closestBlock->relPosition + offset;

		//checking colision box
		bool clear = true;

		//snapping camera axis to chunk's axis
		glm::vec3 collisionXaxis = MathHelper::getVectorDirectionCustomAxis(camera.Front, closestPack->relativeAxisX, closestPack->relativeAxisY, closestPack->relativeAxisZ);
		glm::vec3 collisionYaxis = MathHelper::getVectorDirectionCustomAxis(camera.Up, closestPack->relativeAxisX, closestPack->relativeAxisY, closestPack->relativeAxisZ);
		glm::vec3 collisionZaxis = MathHelper::getVectorDirectionCustomAxis(camera.Right, closestPack->relativeAxisX, closestPack->relativeAxisY, closestPack->relativeAxisZ);

		obj.orientationX = collisionZaxis;
		obj.orientationY = collisionYaxis;
		obj.orientationZ = -collisionXaxis;

		for (int cX = 0; cX <= obj.collisionBox.x; cX++)
		{
			for (int cY = 0; cY <= obj.collisionBox.y; cY++)
			{
				for (int cZ = 0; cZ <= obj.collisionBox.z; cZ++)
				{
					glm::ivec3 position = collisionXaxis * (float)cX + collisionYaxis * (float)cY + collisionZaxis * (float)cZ + (glm::vec3)newBlockRelPos;
					std::vector<Chunk>::iterator chunkIt = closestPack->chunkList.begin();
					while (clear && chunkIt != closestPack->chunkList.end()) {
						std::vector<Block>::iterator entIt = chunkIt->blockList.begin();

						while (clear && entIt != chunkIt->blockList.end()) {

							if (entIt->relPosition == position) {
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

			std::vector<Chunk>::iterator chunk = closestPack->chunkList.begin();

			bool found = false;

			while (!found && chunk != closestPack->chunkList.end()) { //iterate through all chunks in the chunk pack to see if the one the block should be on already exists
				if (chunk->relativePosition == newBlockChunk) {
					found = true;
				}
				else {
					chunk++;
				}
			}

			if (found) {
				obj.relPosition = newBlockRelPos;
				obj.setUp();
				chunk->generatorList.push_back(obj);
				chunk->updateChunkVAO();
			}
			else {
				obj.relPosition = closestBlock->relPosition + offset;
				obj.setUp();

				Chunk newChunk(newBlockChunk.x, newBlockChunk.y, newBlockChunk.z);
				newChunk.generatorList.push_back(obj);
				newChunk.updateChunkVAO();

				closestPack->chunkList.push_back(newChunk);
			}
		}
	}
}

void EnvironmentManager::spawnWire(glm::vec3 absPosition, glm::vec3 relPosition, Entity * closestBlock, Chunk * closestChunk, ChunkPack * closestPack, Wire obj)
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

		std::vector<Chunk>::iterator chunk = closestPack->chunkList.begin();

		bool found = false;

		while (!found && chunk != closestPack->chunkList.end()) { //iterate through all chunks in the chunk pack to see if the one the block should be on already exists
			if (chunk->relativePosition == newBlockChunk) {
				found = true;
			}
			else {
				chunk++;
			}
		}

		if (found) {
			obj.relPosition = newBlockRelPos;
			chunk->wireList.push_back(obj);
			chunk->updateChunkVAO();
		}
		else {

			obj.relPosition = closestBlock->relPosition + offset;

			Chunk newChunk(newBlockChunk.x, newBlockChunk.y, newBlockChunk.z);
			newChunk.wireList.push_back(obj);
			newChunk.updateChunkVAO();

			closestPack->chunkList.push_back(newChunk);
		}

		found = false;

		std::vector<Connection*> connectionsToJoin;

		for each (Connection connect in closestPack->connectionList)
		{
			if (connect.type == obj.type) {
				bool joined = false;
				std::vector<glm::vec3>::iterator point = connect.wirePoints.begin();

				while (!joined && point != connect.wirePoints.end()) {
					if (MathHelper::getVectorLength(*point - (glm::vec3)obj.relPosition) < 1.5) {
						joined = true;
						connectionsToJoin.push_back(&connect);
					}
				}
			}
		}

		switch (connectionsToJoin.size()) {
		case 0: {
			Connection newConn;
			newConn.wirePoints.push_back(obj.relPosition);
			closestPack->connectionList.push_back(newConn);
		}
		case 1: {
			connectionsToJoin.at(0)->wirePoints.push_back(obj.relPosition);
		}


		}

	}
}
