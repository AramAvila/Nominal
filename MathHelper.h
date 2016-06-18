#pragma once
#include "gl/glm/glm.hpp"
#include "entities/Entity.h"
#include <math.h>
#include <iostream>
#include "ChunkPack.h"

class MathHelper {

public:
	static double getVectorLength(glm::vec3 vector);
	//static glm::vec3 getEntityMovement(Object * ent);
	static glm::vec3 getVectorDirection(glm::vec3 disp);
	static glm::ivec3 getVectorDirectionCustomAxis(glm::vec3 disp, glm::vec3 axisX, glm::vec3 axisY, glm::vec3 axisZ);
	static glm::ivec3 getVectorQuadrant(glm::vec3 disp);
	static glm::ivec3 getVectorQuadrantCustomAxis(glm::vec3 disp, glm::vec3 axisX, glm::vec3 axisY, glm::vec3 axisZ);
	static float angleBetween(glm::vec3 a, glm::vec3 b);
	static glm::vec3 objectMeet(Entity * a, Entity * b);
	static float distancePointToLine(glm::vec3 point, glm::vec3 lineStart, glm::vec3 vector);
	static glm::vec3 MathHelper::shortestVecToLine(glm::vec3 VecPlane1, glm::vec3 VecPlane2, glm::vec3 VecLine, glm::vec3 PlanePoint, glm::vec3 LinePoint);
	static glm::vec3 changeReferenceSystem(glm::vec3 point, glm::vec3 newOrigin, glm::vec3 newX, glm::vec3 newY, glm::vec3 newZ);
};