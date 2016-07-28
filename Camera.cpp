#pragma once
#include "Camera.h"

extern std::vector<ChunkPack> chunkPacks;

Camera::Camera(glm::vec3 absPosition, glm::vec3 up, GLfloat yaw, GLfloat pitch, GLfloat roll) : Front(glm::vec3(1.0f, 0.0f, 0.0f)), MovementSpeed(SPEED), MouseSensitivity(SENSITIVTY), Zoom(ZOOM)
{
	this->Position = absPosition;
	this->WorldUp = up;
	this->Yaw = yaw;
	this->Pitch = pitch;
	this->Roll = roll;
	this->updateCameraVectors();
}

// Constructor with scalar values
Camera::Camera(GLfloat posX, GLfloat posY, GLfloat posZ, GLfloat upX, GLfloat upY, GLfloat upZ, GLfloat yaw, GLfloat pitch, GLfloat roll) : Front(glm::vec3(1.0f, 0.0f, 0.0f)), MovementSpeed(SPEED), MouseSensitivity(SENSITIVTY), Zoom(ZOOM){
	this->Position = glm::vec3(posX, posY, posZ);
	this->WorldUp = glm::vec3(upX, upY, upZ);
	this->Yaw = yaw;
	this->Pitch = pitch;
	this->updateCameraVectors();
}

// Returns the view matrix calculated using Eular Angles and the LookAt Matrix
glm::mat4 Camera::GetViewMatrix()
{
	return glm::lookAt(this->Position, this->Position + this->Front, this->Up);
}

// Processes input received from any keyboard-like input system. Accepts input parameter in the form of camera defined ENUM (to abstract it from windowing systems)
void Camera::ProcessKeyboard(Camera_Movement direction, GLfloat deltaTime)
{
	/*std::vector<glm::vec3> movementCorrection;
	std::map<glm::vec3, Chunk*> chunksToProcess; //contains the abs absPosition of all chunks in the pack
	std::map<float, std::pair<Chunk*, ChunkPack*>> selectedChunks; //chunks that are close enough to colide with the player

	//These lists will contain the lists of colliding blocks narrowing a filter each time
	std::vector<glm::vec3> colisionPointsF1; //all blocks in height radius, will contain the center point of the cube
	std::vector<glm::vec3> colisionPointsF2; //all blocks in witdh radius, will contain the shortest vector to the player height line
	std::vector<glm::vec3> colisionPointsF3; //all blocks close enough to be either at top or bottom, will contain the center point of the block

	//camera body
	float cubeMargin = 0.71f;
	float sides = 1.5f;		  //Width of the player
	float totalHeight = 4.0f; //Heigth of the player

	float dist;
	float maxChunkRange = sqrt(3 * Configuration::CHUNK_SIZE ^ 2) + totalHeight; // chunk size = 8  -->  sqrt(8^2 + 8^2 + 8^2) = ~13.85  -->  totalHeight = 4  -->> 13.85 + 4 = ~18

	for (int i = 0; i < chunkPacks.size(); i++) //iterate through all chunk packs to find the 8 closest chunks to the block spawn absPosition
	{
		chunksToProcess = chunkPacks[i].getChunkAbsPositions(); //to do so, we need the absolute absPosition of all the chunks
		for (std::map<glm::vec3, Chunk*>::iterator it = chunksToProcess.begin(); it != chunksToProcess.end(); ++it)
		{
			dist = MathHelper::getVectorLength(it->first - this->Position);
			if (dist < maxChunkRange) {
				selectedChunks[dist] = std::make_pair(it->second, &chunkPacks[i]);
			}
		}
	}

	std::map<float, std::pair<Chunk*, ChunkPack*>>::iterator selChunks = selectedChunks.begin(); //maps are automaticaly sorted by key, therefore, we only need to take the first 8 items
	int c = 0;
	while (selChunks != selectedChunks.end() && c < 9) {
		for (int i = 0; i < selChunks->second.first->objectList.size(); i++)
		{
			if (MathHelper::getVectorLength(selChunks->second.first->objectList[i].absPosition - this->Position) < totalHeight) { //the first filter gets the blocks in height range
				colisionPointsF1.push_back(selChunks->second.first->objectList[i].absPosition);
			}
		}
		selChunks++;
		c++;
	}

	std::vector<glm::vec3>::iterator filter1 = colisionPointsF1.begin();
	glm::vec3 shortestVec;
	float length;
	while (filter1 != colisionPointsF1.end())
	{
		shortestVec = MathHelper::shortestVecToLine(this->Right, this->Front, this->Up, *filter1, this->Position);
		std::cout << shortestVec.x - filter1->x << ", " << shortestVec.y - filter1->y << ", " << shortestVec.z - filter1->z << std::endl;
		length = MathHelper::getVectorLength(shortestVec-*filter1);
		if (length < sides) {
			if (length < cubeMargin) {
				colisionPointsF3.push_back(*filter1);
			}
			else {
				colisionPointsF2.push_back(shortestVec);
			}
		}
		filter1++;
	}
	std::cout << "------------------------------------------------------" << std::endl;
	std::vector<glm::vec3>::iterator filter2 = colisionPointsF2.begin();
	while (filter2 != colisionPointsF2.end()) {
		movementCorrection.push_back(MathHelper::getVectorDirection(*filter2));
		filter2++;
	}

	std::vector<glm::vec3>::iterator filter3 = colisionPointsF3.begin();
	while (filter3 != colisionPointsF3.end()) {
		movementCorrection.push_back(MathHelper::getVectorDirection(this->Position - *filter3));
		filter3++;
	}*/

	glm::vec3 posSides(0, 0, 0);
	glm::vec3 negSides(0, 0, 0);

	/*for (int i = 0; i < movementCorrection.size(); i++)
	{
		if (movementCorrection[i].x != 0) {
			if (movementCorrection[i].x < 0) {
				posSides.x += 1;
			}
			else {
				negSides.x += 1;
			}
		}

		if (movementCorrection[i].y != 0) {
			if (movementCorrection[i].y < 0) {
				posSides.y += 1;
			}
			else {
				negSides.y += 1;
			}
		}

		if (movementCorrection[i].z != 0) {
			if (movementCorrection[i].z < 0) {
				posSides.z += 1;
			}
			else {
				negSides.z += 1;
			}
		}
	}*/

	posSides.x = (posSides.x != 0) ? 0 : 1;
	negSides.x = (negSides.x != 0) ? 0 : 1;

	posSides.y = (posSides.y != 0) ? 0 : 1;
	negSides.y = (negSides.y != 0) ? 0 : 1;

	posSides.z = (posSides.z != 0) ? 0 : 1;
	negSides.z = (negSides.z != 0) ? 0 : 1;

	glm::vec3 frontMovement(0, 0, 0);
	frontMovement.x = (this->Front.x > 0) ? this->Front.x * posSides.x : this->Front.x * negSides.x;
	frontMovement.y = (this->Front.y > 0) ? this->Front.y * posSides.y : this->Front.y * negSides.y;
	frontMovement.z = (this->Front.z > 0) ? this->Front.z * posSides.z : this->Front.z * negSides.z;

	glm::vec3 backMovement(0, 0, 0);
	backMovement.x = (this->Front.x < 0) ? -this->Front.x * posSides.x : -this->Front.x * negSides.x;
	backMovement.y = (this->Front.y < 0) ? -this->Front.y * posSides.y : -this->Front.y * negSides.y;
	backMovement.z = (this->Front.z < 0) ? -this->Front.z * posSides.z : -this->Front.z * negSides.z;

	glm::vec3 rightMovement(0, 0, 0);
	rightMovement.x = (this->Right.x > 0) ? this->Right.x * posSides.x : this->Right.x * negSides.x;
	rightMovement.y = (this->Right.y > 0) ? this->Right.y * posSides.y : this->Right.y * negSides.y;
	rightMovement.z = (this->Right.z > 0) ? this->Right.z * posSides.z : this->Right.z * negSides.z;

	glm::vec3 leftMovement(0, 0, 0);
	leftMovement.x = (this->Right.x < 0) ? -this->Right.x * posSides.x : -this->Right.x * negSides.x;
	leftMovement.y = (this->Right.y < 0) ? -this->Right.y * posSides.y : -this->Right.y * negSides.y;
	leftMovement.z = (this->Right.z < 0) ? -this->Right.z * posSides.z : -this->Right.z * negSides.z;

	glm::vec3 upMovement(0, 0, 0);
	upMovement.x = (this->Up.x > 0) ? this->Up.x * posSides.x : this->Up.x * negSides.x;
	upMovement.y = (this->Up.y > 0) ? this->Up.y * posSides.y : this->Up.y * negSides.y;
	upMovement.z = (this->Up.z > 0) ? this->Up.z * posSides.z : this->Up.z * negSides.z;

	glm::vec3 downMovement(0, 0, 0);
	downMovement.x = (this->Up.x < 0) ? -this->Up.x * posSides.x : -this->Up.x * negSides.x;
	downMovement.y = (this->Up.y < 0) ? -this->Up.y * posSides.y : -this->Up.y * negSides.y;
	downMovement.z = (this->Up.z < 0) ? -this->Up.z * posSides.z : -this->Up.z * negSides.z;

	GLfloat velocity = this->MovementSpeed * deltaTime;

	if (direction == FORWARD)
		this->Position += this->Front * velocity;

	if (direction == BACKWARD)
		this->Position -= this->Front * velocity;

	if (direction == LEFT)
		this->Position -= this->Right * velocity;

	if (direction == RIGHT)
		this->Position += this->Right * velocity;

	if (direction == UP)
		this->Position += this->Up * velocity;
	if (direction == DOWN)
		this->Position -= this->Up * velocity;

	if (direction == ROLL_LEFT)
		this->lastTickMove.x += this->MouseSensitivity * 700;

	if (direction == ROLL_RIGHT)
		this->lastTickMove.x -= this->MouseSensitivity * 700;

	this->updateCameraVectors();
}

// Processes input received from a mouse input system. Expects the offset value in both the x and y direction.
void Camera::ProcessMouseMovement(GLfloat xoffset, GLfloat yoffset, GLboolean constrainPitch)
{
	this->lastTickMove.y -= xoffset;
	this->lastTickMove.z += yoffset;

	// Make sure that when pitch is out of bounds, screen doesn't get flipped
	/*if (constrainPitch)
	{
		if (this->Pitch > 89.0f)
			this->Pitch = 89.0f;
		if (this->Pitch < -89.0f)
			this->Pitch = -89.0f;
	}*/

	// Update Front, Right and Up Vectors using the updated Eular angles
	this->updateCameraVectors();
}

// Processes input received from a mouse scroll-wheel event. Only requires input on the vertical wheel-axis
void Camera::ProcessMouseScroll(GLfloat yoffset)
{
	if (this->spawnDistance >= 1.0f && this->spawnDistance <= 5.0f)
		this->spawnDistance += yoffset;

	if (this->spawnDistance < 1.0f)
		this->spawnDistance = 1.0f;

	if (this->spawnDistance > 5.0f)
		this->spawnDistance = 5.0f;

	this->SpawnPosition = this->Position + this->Front * this->spawnDistance - this->Up / this->spawnDistance;
	this->pickBlock();
}

void Camera::pickBlock() {

	/*float distBlock = 50;
	float prevBlock = 50;

	for (int i = 0; i < staticBlocks.size(); i++)
	{
		distBlock = MathHelper::getVectorLength(staticBlocks[i]->absPosition - this->SpawnPosition);
		if (distBlock < prevBlock) {
			if (prevBlock < 1.0f) {
				if (lastSelectedBlock == NULL) {
					this->lastSelectedBlock = staticBlocks[i];
				}
				this->lastSelectedBlock->selected = 0;
				this->lastSelectedBlock = staticBlocks[i];
				this->lastSelectedBlock->selected = 1;
			}
			prevBlock = distBlock;
		}
	}

	if (prevBlock >= 1.0f && lastSelectedBlock != NULL) {
		this->lastSelectedBlock->selected = 0;
		this->lastSelectedBlock = NULL;
	}-------------------------

	float distBlock;
	float prevDist = 50;

	std::vector<Block *> blocksToEvaluate = staticBlocks;

	for (int i = 0; i < staticBlocks.size(); i++)
	{
		distBlock = MathHelper::getVectorLength(staticBlocks[i]->absPosition - this->Position + this->Front * 2.0f);
		if (distBlock < 2) {
			blocksToEvaluate.push_back(staticBlocks[i]);
		}
	}

	glm::vec3 newSpot;
	float i = 0.01f;
	while (i <= 5) {
		newSpot= this->Position + this->Front * i;
		for (int i = 0; i < blocksToEvaluate.size(); i++)
		{
			distBlock = MathHelper::getVectorLength(blocksToEvaluate[i]->absPosition - newSpot);
			if (distBlock < prevDist) {
				if (prevDist < 1.0f) {
					if (lastSelectedBlock == NULL) {
						this->lastSelectedBlock = staticBlocks[i];
					}
					this->lastSelectedBlock->selected = 0;
					this->lastSelectedBlock = staticBlocks[i];
					this->lastSelectedBlock->selected = 1;
				}
				prevDist = distBlock;
			}
		}
		i += 0.5f;
	}

	if (prevDist >= 1.0f && lastSelectedBlock != NULL) {
		this->lastSelectedBlock->selected = 0;
		this->lastSelectedBlock = NULL;
	}*/
};

// Calculates the front vector from the Camera's (updated) Eular Angles
void Camera::updateCameraVectors()
{

	this->Front = MathHelper::rotatePointArroundAxis(this->Front, this->Up, this->lastTickMove.y * this->MouseSensitivity);
	this->Right = MathHelper::rotatePointArroundAxis(this->Right, this->Up, this->lastTickMove.y * this->MouseSensitivity);

	this->Right = MathHelper::rotatePointArroundAxis(this->Right, this->Front, this->lastTickMove.x * this->MouseSensitivity);
	this->Up = MathHelper::rotatePointArroundAxis(this->Up, this->Front, this->lastTickMove.x * this->MouseSensitivity);

	this->Front = MathHelper::rotatePointArroundAxis(this->Front, this->Right, this->lastTickMove.z * this->MouseSensitivity);
	this->Up = MathHelper::rotatePointArroundAxis(this->Up, this->Right, this->lastTickMove.z * this->MouseSensitivity);

	this->Pitch = MathHelper::getAnglesFromAim(this->Front).first;
	this->Yaw = -MathHelper::getAnglesFromAim(this->Front).second;
	this->Roll = MathHelper::findRoll(this->Front, this->Right);

	this->lastTickMove = glm::vec3(0, 0, 0);
	this->SpawnPosition = this->Position + this->Front * this->spawnDistance - this->Up / this->spawnDistance;

	/*// Calculate the new Front vector
	glm::vec3 front;
	front.x = cos(glm::radians(this->Yaw)) * cos(glm::radians(this->Pitch));
	front.y = sin(glm::radians(this->Pitch));
	front.z = sin(glm::radians(this->Yaw)) * cos(glm::radians(this->Pitch));
	this->Front = glm::normalize(front);
	// Also re-calculate the Right and Up vector
	this->Right = glm::normalize(glm::cross(this->Front, this->WorldUp));  // Normalize the vectors, because their length gets closer to 0 the more you look up or down which results in slower movement.
	this->Up = glm::normalize(glm::cross(this->Right, this->Front));
	this->SpawnPosition = this->Position + this->Front * this->spawnDistance - this->Up / this->spawnDistance;*/
}