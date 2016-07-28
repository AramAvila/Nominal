#pragma once

#include <vector>
#include <map>
#include <math.h>
#include "gl/glew.h"

#include "gl\glm\vec3.hpp"
#include "configuration.h"
#include "MathHelper.h"
#include "entities\Block.h"
#include "entities\Generator.h"
#include "entities\Wire.h"

class Chunk
{
public:
	Chunk();
	Chunk(int x, int y, int z);
	~Chunk();
	glm::ivec3 relativePosition;

	std::vector<Block> blockList; //this list contains all the entities that won't perform any action (Blocks)
	std::vector<Generator> generatorList; //this list contains all the entities will have to do something (Any kind of system: generator, motor, weapon, etc)
	std::vector<Wire> wireList;
	
	int size; //Size of the chunk, not number of items it contains.

	glm::vec3 color;//Chunk color for debugging

	GLuint chunkVAO = 0;
	GLuint chunkVBO = 0;
	GLuint vertexNum = 0; //number of vertexs in the VBO <--- NEEDED FOR RENDERING
	void updateChunkVAO();

	glm::ivec3 getChunkCenter();
};

struct Connection {
	std::vector<glm::vec3> wirePoints;
	std::vector<Generator*> generatorList;
	
	float energy = 0;
	Configuration::CONNECTION_TYPES type;
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
	
	std::vector<Chunk> chunkList;
	std::vector<Connection> connectionList;

	std::map<glm::vec3, Chunk*> getChunkAbsPositions();
};
