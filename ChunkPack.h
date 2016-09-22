#pragma once

#include <vector>
#include <map>
#include <math.h>
#include <memory>
#include "gl/glew.h"

#include "gl\glm\vec3.hpp"
#include "configuration.h"
#include "MathHelper.h"
#include "entities\Block.h"
#include "entities\Generator.h"
#include "entities\Wire.h"
#include "entities\WireHolder.h"
#include "entities\Engine.h"
#include "HelperClasses.h"

class Chunk
{
public:
	Chunk();
	Chunk(int x, int y, int z);
	Chunk(glm::ivec3 position);

	~Chunk();

	glm::ivec3 relativePosition;

	std::vector<HelperClasses::Polygon> polygons;

	std::vector<std::shared_ptr<Entity>> visualEntities;

	std::multimap<glm::ivec3, std::shared_ptr<Entity>> collisionPoints;
			
	int size; //Size of the chunk, not number of items it contains.

	glm::vec3 color;//Chunk color for debugging

	std::unique_ptr<GLuint> chunkVAO;
	std::unique_ptr<GLuint> chunkVBO;

	GLuint chunkVertexNum = 0; //number of vertexs in the VBO <--- NEEDED FOR RENDERING
	void updateChunkVAO();
	bool VAOupdatePending = false;
	

	glm::ivec3 getChunkCenter();
};

class ChunkPack
{
public:
	ChunkPack(float x, float y, float z);
	ChunkPack();
	~ChunkPack();

	glm::vec3 absPosition;
	glm::vec3 relativeAxisX;
	glm::vec3 relativeAxisY;
	glm::vec3 relativeAxisZ;

	float pitch = 0;
	float yaw = 0;
	float roll = 0;
	
	std::vector<std::shared_ptr<Chunk>> chunkList;

	std::vector<std::shared_ptr<HelperClasses::Connection>> connectionList;

	std::vector<std::shared_ptr<Block>> blockList; //this list contains all the entities that won't perform any action (Blocks)
	std::vector<std::shared_ptr<Generator>> generatorList; //this list contains all the entities will have to do something (Any kind of system: generator, motor, weapon, etc)
	std::vector<std::shared_ptr<WireHolder>> wireHolderList;
	std::vector<std::shared_ptr<Engine>> engineList;

	std::pair<GLint, GLint> getHudsVAO(); //first = VAO, second = VertexCount

	std::unique_ptr<GLuint> hudsVAO;
	std::unique_ptr<GLuint> hudsVBO;
	GLuint hudsVertexNum = 0;

	std::map<glm::vec3, Chunk*> getChunkAbsPositions();
	void addColisionPoints(std::map<glm::ivec3, std::shared_ptr<Entity>> newPoints);
	Chunk* getChunkAt(glm::ivec3 position);
};
