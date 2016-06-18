#include "ChunkPack.h"


ChunkPack::ChunkPack(float x, float y, float z)
{
	this->absPosition = glm::vec3(x, y, z);
	this->relativeAxisX = glm::vec3(1, 0, 0);
	this->relativeAxisY = glm::vec3(0, 1, 0);
	this->relativeAxisZ = glm::vec3(0, 0, 1);
}

ChunkPack::ChunkPack()
{
	this->relativeAxisX = glm::vec3(1, 0, 0);
	this->relativeAxisY = glm::vec3(0, 1, 0);
	this->relativeAxisZ = glm::vec3(0, 0, 1);
}


ChunkPack::~ChunkPack()
{
}



/// <summary>
/// Returns the absolute position of the chunks. This position is the center of the chunk!!
/// </summary>
/// <returns>Map containing the position of the chunk's center and a pointer to said chunk</returns>
std::map<glm::vec3, Chunk*> ChunkPack::getChunkAbsPositions()
{
	std::map<glm::vec3, Chunk*> chunkPositions;

	for (int i = 0; i < this->chunkList.size(); i++)
	{
		glm::vec3 relPosition = this->chunkList[i].relativePosition * Configuration::CHUNK_SIZE;
		//Configuration::CHUNK_SIZE / 2 is added to get from the corner of the chunk to it's center
		relPosition += glm::vec3(Configuration::CHUNK_SIZE / 2, Configuration::CHUNK_SIZE / 2, Configuration::CHUNK_SIZE / 2);
		glm::vec3 absPosition = relativeAxisX * relPosition.x + relativeAxisY * relPosition.y + relativeAxisZ * relPosition.z;
		absPosition += this->absPosition;
		chunkPositions[absPosition] = &this->chunkList[i];
	}

	return chunkPositions;
}

//-----------------------

Chunk::Chunk()
{
	this->size = Configuration::CHUNK_SIZE;
}

Chunk::Chunk(int x, int y, int z)
{
	this->relativePosition = glm::ivec3(x, y, z);
	this->size = Configuration::CHUNK_SIZE;
	this->color = glm::vec3(0.0f, 0.0f, 0.0f);
	if (x % 2 == 0) {
		this->color.x = 1;
	}
	if (y % 2 == 0) {
		this->color.y = 1;
	}
	if (z % 2 == 0) {
		this->color.z = 1;
	}
}

Chunk::~Chunk()
{
}

/// <summary>
/// Returns the center of the chunk relative to the origin of the chunkPack
/// </summary>
/// <returns></returns>
glm::ivec3 Chunk::getChunkCenter()
{
	glm::vec3 center = this->relativePosition * Configuration::CHUNK_SIZE;
	//Configuration::CHUNK_SIZE / 2 is added to get from the corner of the chunk to it's center
	center += glm::vec3(Configuration::CHUNK_SIZE / 2, Configuration::CHUNK_SIZE / 2, Configuration::CHUNK_SIZE / 2);
	return center;
}


struct vertex {
	GLfloat positions[3];
	GLfloat normals[3];
	GLfloat textures[2];
};

struct Face { //1-2-3, 2-1-4
	GLfloat v1[8];
	GLfloat v2[8];
	GLfloat v3[8];
	GLfloat v4[8];
};

void Chunk::updateChunkVAO() {

	GLfloat chunkVertices[116736]; //the maximum amount of vertices a chunk can have is 116736 -> 8 * 8 * 8 * 228
	GLfloat x, y, z;
	GLint vertexCount = 0;

	//std::vector<Face> faces;

	int c = 0;
	for (int i = 0; i < this->objectList.size(); i++)
	{
		x = this->objectList[i].relPosition.x;
		y = this->objectList[i].relPosition.y;
		z = this->objectList[i].relPosition.z;

		for (int a = 0; a < objectList[i].body.vertexArray.size(); a = a + 8)
		{
			chunkVertices[c++] = objectList[i].body.vertexArray[a] + x;  //Vertex
			chunkVertices[c++] = objectList[i].body.vertexArray[a + 1] + y;
			chunkVertices[c++] = objectList[i].body.vertexArray[a + 2] + z;

			chunkVertices[c++] = objectList[i].body.vertexArray[a + 3];  //Normals
			chunkVertices[c++] = objectList[i].body.vertexArray[a + 4];
			chunkVertices[c++] = objectList[i].body.vertexArray[a + 5];

			chunkVertices[c++] = objectList[i].body.vertexArray[a + 6];  //Textures
			chunkVertices[c++] = objectList[i].body.vertexArray[a + 7];

			vertexCount++;
		}

		/*							//POSITION						NORMAL			 TEXT. COORDINATES
								// 1      2      3				 1     2      3			 X     Y
		GLfloat vert1[8] = { x - 0.5f, y - 0.5f, z - 0.5f,		0.0f,  0.0f, -1.0f,		0.0f, 0.0f };
		GLfloat vert2[8] = { x + 0.5f, y + 0.5f, z - 0.5f,		0.0f,  0.0f, -1.0f,		1.0f, 1.0f };
		GLfloat vert3[8] = { x + 0.5f, y - 0.5f, z - 0.5f,		0.0f,  0.0f, -1.0f,		1.0f, 0.0f };
		GLfloat vert4[8] = { x - 0.5f, y + 0.5f, z - 0.5f,		0.0f,  0.0f, -1.0f,		0.0f, 1.0f };
		Face f;
		memcpy(f.v1, vert1, 8 * sizeof(GLfloat));
		memcpy(f.v2, vert2, 8 * sizeof(GLfloat));
		memcpy(f.v3, vert3, 8 * sizeof(GLfloat));
		memcpy(f.v4, vert4, 8 * sizeof(GLfloat));
		faces.push_back(f);

		GLfloat vert5[8] = { x - 0.5f, y - 0.5f, z + 0.5f,		0.0f,  0.0f,  1.0f,		0.0f, 0.0f };
		GLfloat vert6[8] = { x + 0.5f, y + 0.5f, z + 0.5f,		0.0f,  0.0f,  1.0f,		1.0f, 1.0f };
		GLfloat vert7[8] = { x + 0.5f, y - 0.5f, z + 0.5f,		0.0f,  0.0f,  1.0f,		1.0f, 0.0f };
		GLfloat vert8[8] = { x - 0.5f, y + 0.5f, z + 0.5f,		0.0f,  0.0f,  1.0f,		0.0f, 1.0f };
		memcpy(f.v1, vert5, 8 * sizeof(GLfloat));
		memcpy(f.v2, vert6, 8 * sizeof(GLfloat));
		memcpy(f.v3, vert7, 8 * sizeof(GLfloat));
		memcpy(f.v4, vert8, 8 * sizeof(GLfloat));
		faces.push_back(f);

		GLfloat vert9[8] = { x - 0.5f, y - 0.5f, z + 0.5f,		-1.0f,  0.0f,  0.0f,	0.0f, 0.0f };
		GLfloat vert10[8] = { x - 0.5f, y + 0.5f, z - 0.5f,		-1.0f,  0.0f,  0.0f,	1.0f, 1.0f };
		GLfloat vert11[8] = { x - 0.5f, y + 0.5f, z + 0.5f,		-1.0f,  0.0f,  0.0f,	1.0f, 0.0f };
		GLfloat vert12[8] = { x - 0.5f, y - 0.5f, z - 0.5f,		-1.0f,  0.0f,  0.0f,	0.0f, 1.0f };
		memcpy(f.v1, vert9, 8 * sizeof(GLfloat));
		memcpy(f.v2, vert10, 8 * sizeof(GLfloat));
		memcpy(f.v3, vert11, 8 * sizeof(GLfloat));
		memcpy(f.v4, vert12, 8 * sizeof(GLfloat));
		faces.push_back(f);

		GLfloat vert13[8] = { x + 0.5f, y - 0.5f, z + 0.5f,		1.0f,  0.0f,  0.0f,		0.0f, 0.0f };
		GLfloat vert14[8] = { x + 0.5f, y + 0.5f, z - 0.5f,		1.0f,  0.0f,  0.0f,		1.0f, 1.0f };
		GLfloat vert15[8] = { x + 0.5f, y + 0.5f, z + 0.5f,		1.0f,  0.0f,  0.0f,		1.0f, 0.0f };
		GLfloat vert16[8] = { x + 0.5f, y - 0.5f, z - 0.5f,		1.0f,  0.0f,  0.0f,		0.0f, 1.0f };
		memcpy(f.v1, vert13, 8 * sizeof(GLfloat));
		memcpy(f.v2, vert14, 8 * sizeof(GLfloat));
		memcpy(f.v3, vert15, 8 * sizeof(GLfloat));
		memcpy(f.v4, vert16, 8 * sizeof(GLfloat));
		faces.push_back(f);

		GLfloat vert17[8] = { x - 0.5f, y - 0.5f, z + 0.5f,		0.0f, -1.0f,  0.0f,		0.0f, 0.0f };
		GLfloat vert18[8] = { x + 0.5f, y - 0.5f, z - 0.5f,		0.0f, -1.0f,  0.0f,		1.0f, 1.0f };
		GLfloat vert19[8] = { x + 0.5f, y - 0.5f, z + 0.5f,		0.0f, -1.0f,  0.0f,		1.0f, 0.0f };
		GLfloat vert20[8] = { x - 0.5f, y - 0.5f, z - 0.5f,		0.0f, -1.0f,  0.0f,		0.0f, 1.0f };
		memcpy(f.v1, vert17, 8 * sizeof(GLfloat));
		memcpy(f.v2, vert18, 8 * sizeof(GLfloat));
		memcpy(f.v3, vert19, 8 * sizeof(GLfloat));
		memcpy(f.v4, vert20, 8 * sizeof(GLfloat));
		faces.push_back(f);

		GLfloat vert21[8] = { x - 0.5f, y + 0.5f, z + 0.5f,		0.0f,  1.0f,  0.0f,		0.0f, 0.0f };
		GLfloat vert22[8] = { x + 0.5f, y + 0.5f, z - 0.5f,		0.0f,  1.0f,  0.0f,		1.0f, 1.0f };
		GLfloat vert23[8] = { x + 0.5f, y + 0.5f, z + 0.5f,		0.0f,  1.0f,  0.0f,		1.0f, 0.0f };
		GLfloat vert24[8] = { x - 0.5f, y + 0.5f, z - 0.5f,		0.0f,  1.0f,  0.0f,		0.0f, 1.0f };
		memcpy(f.v1, vert21, 8 * sizeof(GLfloat));
		memcpy(f.v2, vert22, 8 * sizeof(GLfloat));
		memcpy(f.v3, vert23, 8 * sizeof(GLfloat));
		memcpy(f.v4, vert24, 8 * sizeof(GLfloat));
		faces.push_back(f);
	}

	for (int f = 0; f < faces.size(); f++)
	{
		for (int k = 0; k < 8; k++)
		{
			chunkVertices[a] = faces[f].v1[k];
			a++;
		}
		for (int k = 0; k < 8; k++)
		{
			chunkVertices[a] = faces[f].v2[k];
			a++;
		}
		for (int k = 0; k < 8; k++)
		{
			chunkVertices[a] = faces[f].v3[k];
			a++;
		}
		for (int k = 0; k < 8; k++)
		{
			chunkVertices[a] = faces[f].v2[k];
			a++;
		}
		for (int k = 0; k < 8; k++)
		{
			chunkVertices[a] = faces[f].v1[k];
			a++;
		}
		for (int k = 0; k < 8; k++)
		{
			chunkVertices[a] = faces[f].v4[k];
			a++;
		}*/
	}

	GLuint cVAO, cVBO;
	GLsizeiptr size = vertexCount * 8 * sizeof(GLfloat); // number of vertex * each vertex has 8 values * each value is a float
	glGenVertexArrays(1, &cVAO);
	glGenBuffers(1, &cVBO);
	// Fill buffer
	glBindBuffer(GL_ARRAY_BUFFER, cVBO);
	glBufferData(GL_ARRAY_BUFFER, size, chunkVertices, GL_STATIC_DRAW);
	// Link vertex attributes
	glBindVertexArray(cVAO);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)0); //(vertex atribute array to store, number of values, isNormalized?, size of vertex data, offset from start of vertex data)
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(6 * sizeof(GLfloat)));
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	this->vertexNum = vertexCount;
	this->chunkVAO = cVAO;
	this->chunkVBO = cVBO;
}