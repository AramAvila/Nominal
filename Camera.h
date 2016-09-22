#pragma once

// Std. Includes
#include <vector>

// GL Includes
#include "gl/glew.h"
#include "gl/glm/glm.hpp"
#include "gl/glm/gtc/matrix_transform.hpp"

#include "MathHelper.h"
#include "ChunkPack.h"


// Defines several possible options for camera movement. Used as abstraction to stay away from window-system specific input methods
enum Camera_Movement {
	FORWARD,
	BACKWARD,
	LEFT,
	RIGHT,
	UP,
	DOWN,
	ROLL_LEFT,
	ROLL_RIGHT
};

// Default camera values
const GLfloat YAW = 0.0f;
const GLfloat PITCH = 0.0f;
const GLfloat ROLL = 0.0f;

const GLfloat SPEED = 3.0f;
const GLfloat SENSITIVTY = 0.008f;
const GLfloat ZOOM = 45.0f;


// An abstract camera class that processes input and calculates the corresponding Eular Angles, Vectors and Matrices for use in OpenGL
class Camera
{
public:
	// Camera Attributes
	glm::vec3 Position;
	glm::vec3 Front =	glm::vec3(0, 0, 1);
	glm::vec3 Up =		glm::vec3(0, 1, 0);
	glm::vec3 Right =	glm::vec3(1, 0, 0);
	glm::vec3 WorldUp = glm::vec3(0, 1, 0);

	glm::vec3 SpawnPosition;
	// Eular Angles
	GLfloat Yaw = 0.0f;
	GLfloat Pitch = 0.0f;
	GLfloat Roll = 0.0f;

	// Camera options
	GLfloat MovementSpeed = 10.0f;
	GLfloat MouseSensitivity = 0.008f;
	GLfloat Zoom = 45.0f;

	GLfloat spawnDistance = 3.0f;
	GLfloat PlayerHeight = 4.0f;

	glm::vec3 lastTickMove = glm::vec3(0, 0, 0);

	// Constructor with vectors
	Camera(glm::vec3 absPosition);

	// Returns the view matrix calculated using Eular Angles and the LookAt Matrix
	glm::mat4 GetViewMatrix();

	// Processes input received from any keyboard-like input system. Accepts input parameter in the form of camera defined ENUM (to abstract it from windowing systems)
	void ProcessKeyboard(Camera_Movement direction, GLfloat deltaTime);

	// Processes input received from a mouse input system. Expects the offset value in both the x and y direction.
	void ProcessMouseMovement(GLfloat xoffset, GLfloat yoffset, GLboolean constrainPitch = true);

	// Processes input received from a mouse scroll-wheel event. Only requires input on the vertical wheel-axis
	void ProcessMouseScroll(GLfloat yoffset);

	void pickBlock();

	// Calculates the front vector from the Camera's (updated) Eular Angles
	void updateCameraVectors();
};

