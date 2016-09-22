#include "ChunkPack.h"


ChunkPack::ChunkPack(float x, float y, float z)
{
	this->absPosition = glm::vec3(x, y, z);
	this->relativeAxisX = glm::vec3(1, 0, 0);
	this->relativeAxisY = glm::vec3(0, 1, 0);
	this->relativeAxisZ = glm::vec3(0, 0, 1);

	this->hudsVAO = std::move(std::unique_ptr<GLuint>(new GLuint()));
	this->hudsVBO = std::move(std::unique_ptr<GLuint>(new GLuint()));

	glGenVertexArrays(1, &(*this->hudsVAO));
	glGenBuffers(1, &(*this->hudsVBO));
}

ChunkPack::ChunkPack()
{
	this->relativeAxisX = glm::vec3(1, 0, 0);
	this->relativeAxisY = glm::vec3(0, 1, 0);
	this->relativeAxisZ = glm::vec3(0, 0, 1);
	
	this->hudsVAO = std::move(std::unique_ptr<GLuint>(new GLuint()));
	this->hudsVBO = std::move(std::unique_ptr<GLuint>(new GLuint()));

	glGenVertexArrays(1, &(*this->hudsVAO));
	glGenBuffers(1, &(*this->hudsVBO));
}


ChunkPack::~ChunkPack()
{
	chunkList.clear();
	connectionList.clear();


	hudsVAO.reset();
	hudsVBO.reset();
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
		glm::vec3 relPosition = this->chunkList[i]->relativePosition * Configuration::CHUNK_SIZE;
		//Configuration::CHUNK_SIZE / 2 is added to get from the corner of the chunk to it's center
		relPosition += glm::vec3(Configuration::CHUNK_SIZE / 2, Configuration::CHUNK_SIZE / 2, Configuration::CHUNK_SIZE / 2);
		glm::vec3 absPosition = this->absPosition + relativeAxisX * relPosition.x + relativeAxisY * relPosition.y + relativeAxisZ * relPosition.z;

		chunkPositions[absPosition] = &(*this->chunkList[i]);
	}

	return chunkPositions;
}

void ChunkPack::addColisionPoints(std::map<glm::ivec3, std::shared_ptr<Entity>> newPoints)
{
	Chunk* pointChunk = NULL;

	for each (std::pair<glm::ivec3, std::shared_ptr<Entity>> point in newPoints)
	{

		glm::vec3 a = (glm::vec3)point.first / (float)Configuration::CHUNK_SIZE; //var a only used in the following 4 lines
		a.x = a.x < 0 ? a.x - 1 : a.x;
		a.y = a.y < 0 ? a.y - 1 : a.y; //first chunk in xPos will be 0,0,0 ----- first chunk in xNeg will be -1,0,0
		a.z = a.z < 0 ? a.z - 1 : a.z;
		glm::ivec3 colisionChunk = (glm::ivec3)a;  //casting to integer will truncate the float values  12.35->12 , -15.247->-15
		
		pointChunk = this->getChunkAt(colisionChunk);

		if (pointChunk != NULL) {
			pointChunk->collisionPoints.insert(point);
		}
	}
	
}

Chunk * ChunkPack::getChunkAt(glm::ivec3 position) {

	bool found = false;
	std::vector<std::shared_ptr<Chunk>>::iterator chunkIt = this->chunkList.begin();

	while (!found && chunkIt != this->chunkList.end())
	{
		if ((*chunkIt)->relativePosition == position) {
			found = true;
		}
		else {
			chunkIt++;
		}
	}

	if (found) {
		return &(*(*chunkIt));
	}
	else {
		return NULL;
	}
}

std::pair<GLint, GLint> ChunkPack::getHudsVAO()
{

	struct hudSet {
		glm::vec3 position,
			orientationX,
			orientationY,
			orientationZ;

		std::vector<std::pair<glm::vec2, std::vector<GLfloat>>> models;
	};

	std::vector<hudSet> hudsToDraw;

	for each (std::shared_ptr<Generator> gen in this->generatorList)
	{
		hudSet mSet;

		mSet.position = gen->hud.relativePos;
		mSet.orientationX = gen->hud.relativeX;
		mSet.orientationY = gen->hud.relativeY;
		mSet.orientationZ = gen->orientationZ;

		mSet.models.insert(mSet.models.end(), gen->hud.charArray.begin(), gen->hud.charArray.end());

		hudsToDraw.push_back(mSet);
	}

	for each (std::shared_ptr<Engine> eng in this->engineList)
	{
		hudSet mSet;

		mSet.position = eng->hud.relativePos;
		mSet.orientationX = eng->hud.relativeX;
		mSet.orientationY = eng->hud.relativeY;
		mSet.orientationZ = eng->orientationZ;

		mSet.models.insert(mSet.models.end(), eng->hud.charArray.begin(), eng->hud.charArray.end());

		hudsToDraw.push_back(mSet);
	}

	//Model list complete. Now, build the vertex array

	GLfloat hudVertices[116740];
	GLint vertexCount = 0;
	GLint v = 0;

	for each (hudSet mSet in hudsToDraw)
	{
		glm::vec3 relPosition = mSet.position;

		glm::vec3 axisX = mSet.orientationX;
		glm::vec3 axisY = -mSet.orientationY;
		glm::vec3 axisZ = mSet.orientationZ;

		glm::vec3 point;
		for each(std::pair<glm::vec2, std::vector<GLfloat>> model in mSet.models) {

			for (int c = 0; c < model.second.size(); c += 8)
			{
				point = mSet.position
					+ mSet.orientationX * model.first.x +
					+mSet.orientationY * model.first.y +

					+mSet.orientationX * model.second[c]
					+ mSet.orientationY * model.second[c + 1]
					+ mSet.orientationZ * model.second[c + 2];

				hudVertices[v++] = point.x;
				hudVertices[v++] = point.y;
				hudVertices[v++] = point.z;

				hudVertices[v++] = model.second[c + 3];  //Normals
				hudVertices[v++] = model.second[c + 4];
				hudVertices[v++] = model.second[c + 5];

				hudVertices[v++] = model.second[c + 6];  //Textures
				hudVertices[v++] = model.second[c + 7];

				vertexCount++;
			}
			vertexCount++;

		}
	}
	GLuint hVAO, hVBO;
	if (vertexCount != 0) {

		GLsizeiptr size = vertexCount * 8 * sizeof(GLfloat); // number of vertex * each vertex has 8 values * each value is a float

															 // Fill buffer
		glBindBuffer(GL_ARRAY_BUFFER, *this->hudsVBO);
		glBufferData(GL_ARRAY_BUFFER, size, hudVertices, GL_STATIC_DRAW);
		// Link vertex attributes
		glBindVertexArray(*this->hudsVAO);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)0); //(vertex atribute array to store, number of values, isNormalized?, size of vertex data, offset from start of vertex data)
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
		glEnableVertexAttribArray(2);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(6 * sizeof(GLfloat)));
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);

		/*this->hudsVAO = hVAO;
		this->hudsVBO = hVBO;*/
		this->hudsVertexNum = vertexCount;
	}

	return std::pair<GLint, GLint>(*this->hudsVAO, vertexCount);
}


//-----------------------

Chunk::Chunk()
{
	this->size = Configuration::CHUNK_SIZE;

	this->chunkVAO = std::move(std::unique_ptr<GLuint>(new GLuint()));
	this->chunkVBO = std::move(std::unique_ptr<GLuint>(new GLuint()));

	glGenVertexArrays(1, &(*this->chunkVAO));
	glGenBuffers(1, &(*this->chunkVBO));
	
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

	this->chunkVAO = std::move(std::unique_ptr<GLuint>(new GLuint()));
	this->chunkVBO = std::move(std::unique_ptr<GLuint>(new GLuint()));

	glGenVertexArrays(1, &(*this->chunkVAO));
	glGenBuffers(1, &(*this->chunkVBO));
}

Chunk::Chunk(glm::ivec3 position)
{
	this->relativePosition = position;
	this->size = Configuration::CHUNK_SIZE;
	this->color = glm::vec3(0.0f, 0.0f, 0.0f);
	if (position.x % 2 == 0) {
		this->color.x = 1;
	}
	if (position.y % 2 == 0) {
		this->color.y = 1;
	}
	if (position.z % 2 == 0) {
		this->color.z = 1;
	}

	this->chunkVAO = std::move(std::unique_ptr<GLuint>(new GLuint()));
	this->chunkVBO = std::move(std::unique_ptr<GLuint>(new GLuint()));

	glGenVertexArrays(1, &(*this->chunkVAO));
	glGenBuffers(1, &(*this->chunkVBO));
}


Chunk::~Chunk()
{
	visualEntities.clear();
	collisionPoints.clear();
	
	chunkVAO.reset();
	chunkVBO.reset();

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

	VAOupdatePending = false;

	struct modelSet {
		glm::vec3 position,
			orientationX,
			orientationY,
			orientationZ;

		std::vector<Model> models;
	};

	std::vector<modelSet> modelsToDraw;

	for each (std::shared_ptr<Entity> block in this->visualEntities)
	{
		modelSet mSet;

		mSet.position = block->relPosition;
		mSet.orientationX = block->orientationX;
		mSet.orientationY = block->orientationY;
		mSet.orientationZ = block->orientationZ;

		mSet.models.insert(mSet.models.end(), block->bodyModels.begin(), block->bodyModels.end());

		modelsToDraw.push_back(mSet);
	}


	/*for each (std::shared_ptr<WireHolder> wHolder in this->wireHolderList)
	{

		for (int i = 0; i < 4; i++) {
			if (wHolder->wireList[i] != NULL) {
				modelSet mSet;

				mSet.position = (glm::vec3)wHolder->position + wHolder->disp[i];

				mSet.orientationX = glm::vec3(1, 0, 0);
				mSet.orientationY = glm::vec3(0, 1, 0);
				mSet.orientationZ = glm::vec3(0, 0, 1);

				mSet.models.push_back(wHolder->wireList[i]->nodeModel);
				mSet.models.push_back(wHolder->wireList[i]->connectedModel);


				modelsToDraw.push_back(mSet);
			}

		}
	}*/


	//Model list complete. Now, build the vertex array

	GLfloat chunkVertices[200000];
	GLint vertexCount = 0;
	GLint v = 0;
	glm::vec3 point;

	for each (modelSet mSet in modelsToDraw)
	{
		glm::vec3 relPosition = mSet.position;

		glm::vec3 axisX = mSet.orientationX;
		glm::vec3 axisY = mSet.orientationY;
		glm::vec3 axisZ = mSet.orientationZ;
			
		for each(Model model in mSet.models) {
			for (int c = 0; c < model.vertexArray.size(); c += 8)
			{
				point = mSet.position + mSet.orientationX * model.vertexArray[c]
					+ mSet.orientationY * model.vertexArray[c + 1]
					+ mSet.orientationZ * model.vertexArray[c + 2];

				chunkVertices[v++] = point.x;
				chunkVertices[v++] = point.y;
				chunkVertices[v++] = point.z;

				chunkVertices[v++] = model.vertexArray[c + 3];  //Normals
				chunkVertices[v++] = model.vertexArray[c + 4];
				chunkVertices[v++] = model.vertexArray[c + 5];

				chunkVertices[v++] = model.vertexArray[c + 6];  //Textures
				chunkVertices[v++] = model.vertexArray[c + 7];

				vertexCount++;
			}
		}
	}

	for each (HelperClasses::Polygon pol in this->polygons)
	{

		for (int c = 0; c < pol.vertexArray.size(); c += 8)
		{

			chunkVertices[v++] = pol.vertexArray[c];
			chunkVertices[v++] = pol.vertexArray[c+1];
			chunkVertices[v++] = pol.vertexArray[c+2];

			chunkVertices[v++] = pol.vertexArray[c + 3];  //Normals
			chunkVertices[v++] = pol.vertexArray[c + 4];
			chunkVertices[v++] = pol.vertexArray[c + 5];

			chunkVertices[v++] = pol.vertexArray[c + 6];  //Textures
			chunkVertices[v++] = pol.vertexArray[c + 7];

			vertexCount++;
		}
	}

	GLsizeiptr size = vertexCount * 8 * sizeof(GLfloat); // number of vertex * each vertex has 8 values * each value is a float

	// Fill buffer
	glBindBuffer(GL_ARRAY_BUFFER, *this->chunkVBO);
	glBufferData(GL_ARRAY_BUFFER, size, chunkVertices, GL_STATIC_DRAW);
	// Link vertex attributes
	glBindVertexArray(*this->chunkVAO);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)0); //(vertex atribute array to store, number of values, isNormalized?, size of vertex data, offset from start of vertex data)
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(6 * sizeof(GLfloat)));
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	this->chunkVertexNum = vertexCount;
}