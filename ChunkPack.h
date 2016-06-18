#pragma once

#include <vector>
#include <map>
#include <math.h>
#include "gl/glew.h"

#include "gl\glm\vec3.hpp"
#include "configuration.h"
#include "MathHelper.h"


class Chunk
{
public:
	Chunk();
	Chunk(int x, int y, int z);
	~Chunk();
	glm::ivec3 relativePosition;

	std::vector<Entity> objectList; //this list contains all the entities that won't perform any action (Blocks)
	std::vector<Entity> systemList; //this list contains all the entities will have to do something (Any kind of system: generator, motor, weapon, etc)

	//Size of the chunk, not number of items it contains.
	int size;

	//Chunk color for debugging
	glm::vec3 color;

	GLuint chunkVAO = 0;
	GLuint chunkVBO = 0;
	GLuint vertexNum = 0; //number of vertexs in the VBO <--- NEEDED FOR RENDERING
	void updateChunkVAO();

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
	std::vector<Chunk> chunkList;
	std::map<glm::vec3, Chunk*> getChunkAbsPositions();
};
