#pragma once

#include <map>
#include <string>
#include <unordered_map>
#include <math.h>

#include "GlobalLight.h"
#include "PointLight.h"
#include "Renderer.h"
#include "MathHelper.h"
#include "textures\SOIL.h"
#include "gl\glm\vec3.hpp"
#include "ChunkPack.h"
#include "entities\Entity.h"

class EnvironmentManager
{
public:
	EnvironmentManager();
	~EnvironmentManager();
	
	void processObjectSpawn(float x, float y, float z, Entity obj);
	void deleteModel(float x, float y, float z);


	GLuint skyBoxTexture[5];
	GLuint skyBoxVAO;

	void setUpSkyBox();

	void loadStar(GlobalLight star);
	void updateDepthMap(GlobalLight * star);

	void loadPointLight(PointLight light);
	void loadPointLightList(PointLight light[5]);
	void updateDepthCubeMap(PointLight * light);

	//void moveEntities();

private:	
	void spawnCube(glm::vec3 absPosition, glm::vec3 relPosition, Entity* closestBlock, Chunk* closestChunk, ChunkPack* closestPack, Entity obj);
	//void spawnGenerator(glm::vec3 absPosition, glm::vec3 relPosition, Object* closestBlock, Chunk* closestChunk, ChunkPack* closestPack, Object obj);

};