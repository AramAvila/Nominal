#pragma once
#include "gl/glm/glm.hpp"
#include "entities/Entity.h"
#include <math.h>
#include <iostream>
#include "ChunkPack.h"

const double PI = 3.1415926535897932384626433832795028841971693993751058209749445923078164062862089986280348253421170679821480865132823066470938446095505822317253594081284811174502;

class MathHelper {

public:
	static double getVectorLength(glm::vec3 vector);
	//static glm::vec3 getEntityMovement(Object * ent);
	static glm::vec3 getVectorDirection(glm::vec3 disp);
	static glm::ivec3 getVectorDirectionCustomAxis(glm::vec3 disp, glm::vec3 axisX, glm::vec3 axisY, glm::vec3 axisZ);
	static glm::ivec3 getVectorQuadrant(glm::vec3 disp);
	static glm::ivec3 getVectorQuadrantCustomAxis(glm::vec3 disp, glm::vec3 axisX, glm::vec3 axisY, glm::vec3 axisZ);
	static float angleBetween(glm::vec3 a, glm::vec3 b);
	static float angleBetweenNormal(glm::vec3 x, glm::vec3 y, glm::vec3 n);
	static glm::vec3 objectMeet(Entity * a, Entity * b);
	static float distancePointToLine(glm::vec3 point, glm::vec3 lineStart, glm::vec3 vector);
	static glm::vec3 shortestVecToLine(glm::vec3 VecPlane1, glm::vec3 VecPlane2, glm::vec3 VecLine, glm::vec3 PlanePoint, glm::vec3 LinePoint);
	static glm::vec3 changeReferenceSystem(glm::vec3 point, glm::vec3 newOrigin, glm::vec3 newX, glm::vec3 newY, glm::vec3 newZ);
	static glm::vec3 rotatePointArroundAxis(glm::vec3 point, glm::vec3 axis, double angle);
	static std::pair<double, double> getAnglesFromAim(glm::vec3 viewAxis);
	static double findRoll(glm::vec3 axisUp, glm::vec3 axisFront);
};