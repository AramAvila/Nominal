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
	glm::vec3 position;
	glm::vec3 normal;
	glm::vec2 texture;
};

struct face {
	vertex v1;
	vertex v2;
	vertex v3;

	bool v1r = false;
	bool v2r = false;
	bool v3r = false;
};

void Chunk::updateChunkVAO() {

	GLfloat x, y, z;
	std::vector<face> faces;

	for (int i = 0; i < this->blockList.size(); i++)
	{
		x = this->blockList[i].relPosition.x;
		y = this->blockList[i].relPosition.y;
		z = this->blockList[i].relPosition.z;

		for (int a = 0; a < blockList[i].body.vertexArray.size(); a += 24)
		{
			vertex v1, v2, v3;

			v1.position = glm::vec3(blockList[i].body.vertexArray[a] + x, blockList[i].body.vertexArray[a + 1] + y, blockList[i].body.vertexArray[a + 2] + z);
			v1.normal = glm::vec3(blockList[i].body.vertexArray[a + 3], blockList[i].body.vertexArray[a + 4], blockList[i].body.vertexArray[a + 5]);
			v1.texture = glm::vec2(blockList[i].body.vertexArray[a + 6], blockList[i].body.vertexArray[a + 7]);

			v2.position = glm::vec3(blockList[i].body.vertexArray[a + 8] + x, blockList[i].body.vertexArray[a + 9] + y, blockList[i].body.vertexArray[a + 10] + z);
			v2.normal = glm::vec3(blockList[i].body.vertexArray[a + 11], blockList[i].body.vertexArray[a + 12], blockList[i].body.vertexArray[a + 13]);
			v2.texture = glm::vec2(blockList[i].body.vertexArray[a + 14], blockList[i].body.vertexArray[a + 15]);

			v3.position = glm::vec3(blockList[i].body.vertexArray[a + 16] + x, blockList[i].body.vertexArray[a + 17] + y, blockList[i].body.vertexArray[a + 18] + z);
			v3.normal = glm::vec3(blockList[i].body.vertexArray[a + 19], blockList[i].body.vertexArray[a + 20], blockList[i].body.vertexArray[a + 21]);
			v3.texture = glm::vec2(blockList[i].body.vertexArray[a + 22], blockList[i].body.vertexArray[a + 23]);

			std::vector<face>::iterator faceIter = faces.end(); //loop condition skipped here!!
			bool repeated = false;
			bool v1IsThere, v2IsThere, v3IsThere = false;

			while (!repeated && faceIter != faces.end()) {

				face f = *faceIter;
				if (MathHelper::angleBetween(f.v1.normal, v1.normal) >= 3) {

					std::cout << "OPOSITE FACES!  ";
					/*if (f.v1.position == v1.position || f.v2.position == v1.position || f.v3.position == v1.position) { v1IsThere = true; }
					if (f.v1.position == v2.position || f.v2.position == v2.position || f.v3.position == v2.position) { v2IsThere = true; }
					if (f.v1.position == v3.position || f.v2.position == v3.position || f.v3.position == v3.position) { v3IsThere = true; }*/
					
					if (f.v1.position == v1.position || f.v1.position == v2.position || f.v1.position == v3.position) { 
						f.v1r = true;
					}
					if (f.v2.position == v1.position || f.v2.position == v2.position || f.v2.position == v3.position) {
						f.v2r = true;
					}
					if (f.v3.position == v1.position || f.v3.position == v2.position || f.v3.position == v3.position) {
						f.v3r = true; 
					}
				}

				/*if (v1IsThere && v2IsThere && v3IsThere) {
					repeated = true;
				}
				else {*/

					faceIter++;
				//}
			}

			face face;
			face.v1 = v1;
			face.v2 = v2;
			face.v3 = v3;
			faces.push_back(face);
		};
	}
	/*
	std::vector<int> repeatedFaces;

	for (int a = 0; a < faces.size(); a++) { //find repeated faces
		for (int b = a + 1; b < faces.size(); b++) {

			glm::vec3 nA = glm::normalize(faces[a].v1.normal);
			glm::vec3 nB = glm::normalize(faces[b].v1.normal);
			//std::cout << (vA + vB).x << ", " << (vA + vB).y << ", " << (vA + vB).z << " -- "  << (vA + vB).length() << std::endl;
			if (faces[a].v1.position == faces[b].v1.position &&
				faces[a].v2.position == faces[b].v2.position &&
				faces[a].v3.position == faces[b].v3.position &&
				MathHelper::getVectorLength(nA + nB) == 0) //if the two vertices are on the same spot and have oposite normals...
			{
				repeatedFaces.push_back(a);
				repeatedFaces.push_back(b);
			}
		}
	}
	std::cout << "Repeated vertices: " << repeatedFaces.size() << std::endl;
	for (int c = repeatedFaces.size() - 1; c >= 0; c--) //remove repeated faces
	{
		faces.erase(faces.begin() + repeatedFaces[c]);
	}*/

	GLfloat chunkVertices[116740]; //the maximum amount of vertices a chunk can have is 116736 -> 4 * 4 * 4 * 228
	GLint vertexCount = faces.size() * 3;
	int c = 0;
	for (int a = 0; a < faces.size(); a++) {

		if (!faces[a].v1r || !faces[a].v2r || !faces[a].v3r) {
			chunkVertices[c++] = faces[a].v1.position.x;
			chunkVertices[c++] = faces[a].v1.position.y;
			chunkVertices[c++] = faces[a].v1.position.z;

			chunkVertices[c++] = faces[a].v1.normal.x;
			chunkVertices[c++] = faces[a].v1.normal.y;
			chunkVertices[c++] = faces[a].v1.normal.z;

			chunkVertices[c++] = faces[a].v1.texture.x;
			chunkVertices[c++] = faces[a].v1.texture.y;

			chunkVertices[c++] = faces[a].v2.position.x;
			chunkVertices[c++] = faces[a].v2.position.y;
			chunkVertices[c++] = faces[a].v2.position.z;

			chunkVertices[c++] = faces[a].v2.normal.x;
			chunkVertices[c++] = faces[a].v2.normal.y;
			chunkVertices[c++] = faces[a].v2.normal.z;

			chunkVertices[c++] = faces[a].v2.texture.x;
			chunkVertices[c++] = faces[a].v2.texture.y;

			chunkVertices[c++] = faces[a].v3.position.x;
			chunkVertices[c++] = faces[a].v3.position.y;
			chunkVertices[c++] = faces[a].v3.position.z;

			chunkVertices[c++] = faces[a].v3.normal.x;
			chunkVertices[c++] = faces[a].v3.normal.y;
			chunkVertices[c++] = faces[a].v3.normal.z;

			chunkVertices[c++] = faces[a].v3.texture.x;
			chunkVertices[c++] = faces[a].v3.texture.y;
		}
		
	}

	for (int i = 0; i < this->generatorList.size(); i++)
	{
		glm::vec3 relPosition = this->generatorList[i].relPosition;
		glm::vec3 axisX = this->generatorList[i].orientationX;
		glm::vec3 axisY = this->generatorList[i].orientationY;
		glm::vec3 axisZ = this->generatorList[i].orientationZ;
		glm::vec3 point;
		for (int a = 0; a < generatorList[i].body.vertexArray.size(); a = a + 8)
		{
			point = relPosition + axisX * generatorList[i].body.vertexArray[a]
								+ axisY * generatorList[i].body.vertexArray[a + 1]
								+ axisZ * generatorList[i].body.vertexArray[a + 2];

			chunkVertices[c++] = point.x;
			chunkVertices[c++] = point.y;
			chunkVertices[c++] = point.z;
			
			chunkVertices[c++] = generatorList[i].body.vertexArray[a + 3];  //Normals
			chunkVertices[c++] = generatorList[i].body.vertexArray[a + 4];
			chunkVertices[c++] = generatorList[i].body.vertexArray[a + 5];

			chunkVertices[c++] = generatorList[i].body.vertexArray[a + 6];  //Textures
			chunkVertices[c++] = generatorList[i].body.vertexArray[a + 7];

			vertexCount++;
		}
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