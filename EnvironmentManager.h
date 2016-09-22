#pragma once

#include <map>
#include <string>
#include <unordered_map>
#include <math.h>
#include <memory>

#include "GlobalLight.h"
#include "PointLight.h"
#include "Renderer.h"
#include "MathHelper.h"
#include "textures\SOIL.h"
#include "gl\glm\vec3.hpp"
#include "ChunkPack.h"
#include "HelperClasses.h"
#include "Assets.h"

#include "entities\EntityManager.h"

class EnvironmentManager
{
public:
	EnvironmentManager();
	~EnvironmentManager();
	
	void buildEntityFromPreview(EntityManager::previewModelData * preview);

	bool snappedStart = false;
	void setNewPreview(EntityManager::ENTITY_LIST entity, glm::vec3 startPoint);
	void updatePreview();
	
	void processObjectSpawn(float x, float y, float z, EntityManager::ENTITY_LIST entity);
	void deleteModel(float x, float y, float z);


	GLuint skyBoxTexture[5];
	GLuint skyBoxVAO;

	void setUpSkyBox();

	void loadStar(std::shared_ptr<GlobalLight> star);
	void updateDepthMap(std::shared_ptr<GlobalLight> star);

	void loadPointLight(std::shared_ptr<PointLight> light);
	void loadPointLightList(std::shared_ptr<PointLight> light[5]);
	void updateDepthCubeMap(std::shared_ptr<PointLight> light);

	//void moveEntities();

private:
	void polygonSpawn(EntityManager::previewModelData * preview);
	void modelSpawn(EntityManager::previewModelData * preview);
	void linearSpawn(EntityManager::previewModelData * preview);
	glm::vec3 snapPoint(glm::vec3 pointToSnap, float gridSize);

	void spawnCube(glm::vec3 absPosition, glm::vec3 relPosition, Entity* closestBlock, Chunk* closestChunk, ChunkPack* closestPack, std::shared_ptr<Entity> obj);
	void spawnGenerator(glm::vec3 absPosition, glm::vec3 relPosition, Entity* closestBlock, Chunk* closestChunk, ChunkPack* closestPack, std::shared_ptr<Generator> obj);
	void spawnWire(glm::vec3 absPosition, glm::vec3 relPosition, WireHolder * closestWH, Entity* closestBlock, Chunk* closestChunk, ChunkPack* closestPack, std::shared_ptr<Wire> obj);
	void spawnEngine(glm::vec3 absPosition, glm::vec3 relPosition, Entity* closestBlock, Chunk* closestChunk, ChunkPack* closestPack, std::shared_ptr<Engine> obj);

};