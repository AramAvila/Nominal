#pragma once
#include "Camera.h"

extern std::vector<std::shared_ptr<ChunkPack>>* chunkPacks;

Camera::Camera(glm::vec3 absPosition)
{
	this->Position = absPosition;
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

	GLfloat velocity = this->MovementSpeed * deltaTime;

	if (direction == FORWARD)
		this->Position += this->Front * velocity;

	if (direction == BACKWARD)
		this->Position -= this->Front * velocity;

	if (direction == LEFT)
		this->Position += this->Right * velocity;

	if (direction == RIGHT)
		this->Position -= this->Right * velocity;

	if (direction == UP)
		this->Position += this->Up * velocity;
	if (direction == DOWN)
		this->Position -= this->Up * velocity;

	if (direction == ROLL_LEFT)
		this->lastTickMove.z -= this->MouseSensitivity * 5;

	if (direction == ROLL_RIGHT)
		this->lastTickMove.z += this->MouseSensitivity * 5;

	this->updateCameraVectors();
}

// Processes input received from a mouse input system. Expects the offset value in both the x and y direction.
void Camera::ProcessMouseMovement(GLfloat xoffset, GLfloat yoffset, GLboolean constrainPitch)
{
	this->lastTickMove.y -= xoffset * this->MouseSensitivity;
	this->lastTickMove.x -= yoffset * this->MouseSensitivity;

	// Update Front, Right and Up Vectors using the updated Eular angles
	this->updateCameraVectors();
}


void Camera::ProcessMouseScroll(GLfloat yoffset)
{

}

void Camera::pickBlock() {
};

void Camera::updateCameraVectors()
{

	this->Front = MathHelper::rotatePointArroundAxis(this->Front, this->Up, this->lastTickMove.y);
	this->Right = MathHelper::rotatePointArroundAxis(this->Right, this->Up, this->lastTickMove.y);

	this->Right = MathHelper::rotatePointArroundAxis(this->Right, this->Front, this->lastTickMove.z);
	this->Up = MathHelper::rotatePointArroundAxis(this->Up, this->Front, this->lastTickMove.z);

	this->Front = MathHelper::rotatePointArroundAxis(this->Front, this->Right, this->lastTickMove.x);
	this->Up = MathHelper::rotatePointArroundAxis(this->Up, this->Right, this->lastTickMove.x);

	this->Pitch = MathHelper::getAnglesFromAim(this->Front).first;
	this->Yaw = MathHelper::getAnglesFromAim(this->Front).second;
	this->Roll = MathHelper::findRoll(this->Front, this->Right);

	this->lastTickMove = glm::vec3(0, 0, 0);
	this->SpawnPosition = glm::vec3(this->Position.x, this->Position.y, this->Position.z) + this->Front * this->spawnDistance - this->Up*this->spawnDistance / 4.0f;


	float dist = Configuration::MAXIMUM_JOIN_RANGE * 2; //just to validate the first iteration of the if

	Chunk* closestChunk = NULL;
	ChunkPack* closestPack = NULL;
	glm::vec3 closestRelPosition;
	glm::vec3 shortestDistanceToPol;
	HelperClasses::Polygon * closestPoligon = NULL;
	glm::vec3 shortestVec;

	for (std::vector<std::shared_ptr<ChunkPack>>::iterator chunkPack = chunkPacks->begin(); chunkPack != chunkPacks->end(); chunkPack++)
	{
		glm::vec3 relPosition = MathHelper::changeReferenceSystem(this->Position, (*chunkPack)->absPosition, (*chunkPack)->relativeAxisX, (*chunkPack)->relativeAxisY, (*chunkPack)->relativeAxisZ);
		glm::vec3 relFront = MathHelper::changeReferenceSystem(this->Front, (*chunkPack)->absPosition, (*chunkPack)->relativeAxisX, (*chunkPack)->relativeAxisY, (*chunkPack)->relativeAxisZ);
		
		for (std::vector<std::shared_ptr<Chunk>>::iterator chunk = (*chunkPack)->chunkList.begin(); chunk != (*chunkPack)->chunkList.end(); chunk++)
		{
			//if the distance between the center of the chunk and the point is less than the lentgth of the diagonal of the chunk plus the join range of the cube, the poligon will be in range to join the chunk
			glm::vec3 chunkCenter = (*chunk)->getChunkCenter();
			glm::vec3 cameraToChunkCenter = chunkCenter - relPosition;

			std::cout << MathHelper::angleBetween(relFront, cameraToChunkCenter) << std::endl;

			if (MathHelper::getVectorLength(cameraToChunkCenter) < sqrt(3 * pow(Configuration::CHUNK_SIZE / 2, 2)) + Configuration::MAXIMUM_JOIN_RANGE &&
				MathHelper::angleBetween(relFront, cameraToChunkCenter) < MathHelper::PI) 
			{
				std::cout << "CHUNK IN RANGE!!!" << std::endl;
				//if (MathHelper::getVectorLength((glm::vec3)(*chunk)->getChunkCenter() - relPosition) < sqrt(3 * pow(Configuration::CHUNK_SIZE / 2, 2)) + Configuration::MAXIMUM_JOIN_RANGE) {
					//if we entered this loop is becouse the chunk is close enough to the start point of the poligon, so that the poligon will have to join the chunk
					//iterate through all of the poligons in the chunk to see if there is any close enough
				/*for each (HelperClasses::Polygon pol in (*chunk)->polygons) {

					glm::vec3 polDiagonal = pol.end - pol.start;

					polDiagonal.x = polDiagonal.x / abs(polDiagonal.x);
					polDiagonal.y = polDiagonal.y / abs(polDiagonal.y);
					polDiagonal.z = polDiagonal.z / abs(polDiagonal.z);

					glm::vec3 polMarginsStart = pol.start - glm::vec3(Configuration::MAXIMUM_JOIN_RANGE * polDiagonal.x, Configuration::MAXIMUM_JOIN_RANGE * polDiagonal.y, Configuration::MAXIMUM_JOIN_RANGE * polDiagonal.z);
					glm::vec3 polMarginsEnd = pol.end + glm::vec3(Configuration::MAXIMUM_JOIN_RANGE * polDiagonal.x, Configuration::MAXIMUM_JOIN_RANGE * polDiagonal.y, Configuration::MAXIMUM_JOIN_RANGE * polDiagonal.z);

					if (MathHelper::isInsideParalelogram(relPosition, polMarginsStart, polMarginsEnd)) {
						shortestVec = MathHelper::findVectorPointToParalelogram(relPosition, pol.start, pol.end);
						if (shortestVec.length() < dist) {
							dist = shortestVec.length();
							closestChunk = &(*(*chunk));
							closestPack = &(*(*chunkPack));
							closestRelPosition = relPosition;
							shortestDistanceToPol = shortestVec;
							closestPoligon = &pol;
						}
					}
				}*/
			}
		}
	};
	std::cout << "Done..." << std::endl;

	/*glm::vec3 closestAbs = closestRelPosition + shortestVec;
	if (closestChunk != NULL) {
		this->SpawnPosition = closestPack->absPosition + closestAbs.x * closestPack->relativeAxisX + closestAbs.y * closestPack->relativeAxisZ + closestAbs.z * closestPack->relativeAxisZ;
	}*/
}