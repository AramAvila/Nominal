#include "MathHelper.h"

using namespace std;

const double MathHelper::PI = 3.1415926535897932384626433832795028841971693993751058209749445923078164062862089986280348253421170679821480865132823066470938446095505822317253594081284811174502;

double MathHelper::getVectorLength(glm::vec3 vector) {

	/*double dist = pow(vector.x, 2) + pow(vector.y, 2) + pow(vector.z, 2);

	return sqrt(dist);*/

	return sqrt(pow(vector.x, 2) + pow(vector.y, 2) + pow(vector.z, 2));
}

/*glm::vec3 MathHelper::getEntityMovement(Object * ent) {
	glm::vec3 move;
	move.x = (ent->absPosition.x - ent->destination.x); //x
	move.y = (ent->absPosition.y - ent->destination.y); //y
	move.z = (ent->absPosition.z - ent->destination.z); //z

	move = glm::normalize(move);
	move.x = move.x * ent->speed;
	move.y = move.y * ent->speed;
	move.z = move.z * ent->speed;

	return move;
}*/


/// <summary>
/// Returns the direction that a vector is pointing (up -> (0,1,0), front -> (1,0,0) ... ) 
/// </summary>
/// <param name="disp">Vector</param>
/// <returns></returns>
glm::vec3 MathHelper::getVectorDirection(glm::vec3 disp) {

	glm::vec3 orientation;

	double angleX = glm::degrees(angleBetween(disp, glm::vec3(1, 0, 0)));
	double angleY = glm::degrees(angleBetween(disp, glm::vec3(0, 1, 0)));
	double angleZ = glm::degrees(angleBetween(disp, glm::vec3(0, 0, 1)));

	if (angleX < 45) {
		orientation.x = 1;
	}
	else if (angleX > 135) {
		orientation.x = -1;
	}

	if (angleY < 45) {
		orientation.y = 1;
	}
	else if (angleY > 135) {
		orientation.y = -1;
	}

	if (angleZ < 45) {
		orientation.z = 1;
	}
	else if (angleZ > 135) {
		orientation.z = -1;
	}
	return orientation;
}

/// <summary>
/// Returns de direction that a vector is pointing. The axis that define "up, down" are defined by the 3 axis given
/// </summary>
/// <param name="disp">The disp.</param>
/// <param name="axisX">The axis x.</param>
/// <param name="axisY">The axis y.</param>
/// <param name="axisZ">The axis z.</param>
/// <returns></returns>
glm::ivec3 MathHelper::getVectorDirectionCustomAxis(glm::vec3 disp, glm::vec3 axisX, glm::vec3 axisY, glm::vec3 axisZ) {

	glm::ivec3 orientation;

	double angleX = glm::degrees(angleBetween(disp, axisX));
	double angleY = glm::degrees(angleBetween(disp, axisY));
	double angleZ = glm::degrees(angleBetween(disp, axisZ));

	double minusAngleX = glm::degrees(angleBetween(disp, -axisX));
	double minusAngleY = glm::degrees(angleBetween(disp, -axisY));
	double minusAngleZ = glm::degrees(angleBetween(disp, -axisZ));

	if (angleX < 45) {
		orientation.x = 1;
	}
	else if (angleX > 135) {
		orientation.x = -1;
	}

	if (angleY < 45) {
		orientation.y = 1;
	}
	else if (angleY > 135) {
		orientation.y = -1;
	}

	if (angleZ < 45) {
		orientation.z = 1;
	}
	else if (angleZ > 135) {
		orientation.z = -1;
	}
	return orientation;
}

/// <summary>
/// Returns the angle between 2 vectors. In radians.
/// </summary>
/// <param name="a">First vector.</param>
/// <param name="b">Second vector.</param>
/// <returns></returns>
float inline MathHelper::angleBetween(glm::vec3 a, glm::vec3 b) {
	/*glm::vec3 nA = glm::normalize(a);
	glm::vec3 nB = glm::normalize(b);
	return acos(glm::dot(nA, nB));*/

	return acos(glm::dot(glm::normalize(a), glm::normalize(b)));
}

float MathHelper::angleBetweenNormal(glm::vec3 a, glm::vec3 b, glm::vec3 n) {
	a = glm::normalize(a);
	b = glm::normalize(b);
	n = glm::normalize(n);

	float dot = a.x*b.x + a.y*b.y + a.z*b.z;
	float det = a.x*b.y*n.z + b.x*n.y*a.z + n.x*a.y*b.z - a.z*b.y*n.x - b.z*n.y*a.x - n.z*a.y*b.x;
	float angle = atan2(det, dot);
	return angle;
}



/// <summary>
/// Returns the quadrant that a vector is pointing to
/// </summary>
/// <param name="disp">Vector</param>
/// <returns></returns>
glm::ivec3 MathHelper::getVectorQuadrant(glm::vec3 disp) {

	glm::ivec3 orientation(0, 0, 0);

	if (disp.x != 0 && disp.y != 0 && disp.z != 0) {
		orientation.x = disp.x < 0 ? -1 : 1;
		orientation.y = disp.y < 0 ? -1 : 1;
		orientation.z = disp.z < 0 ? -1 : 1;
	}

	return orientation;
}

/// <summary>
/// ------------------ NOT WORKING ---------------------
/// Returns de quadrant that a vector is pointing. The quadrants are defined by the 3 axis given
/// </summary>
/// <param name="disp">The disp.</param>
/// <param name="axisX">The axis x.</param>
/// <param name="axisY">The axis y.</param>
/// <param name="axisZ">The axis z.</param>
/// <returns></returns>
glm::ivec3 MathHelper::getVectorQuadrantCustomAxis(glm::vec3 disp, glm::vec3 axisX, glm::vec3 axisY, glm::vec3 axisZ) {

	glm::ivec3 orientation;

	double angleX = glm::degrees(angleBetween(disp, axisX));
	double angleY = glm::degrees(angleBetween(disp, axisY));
	double angleZ = glm::degrees(angleBetween(disp, axisZ));

	if (angleX < 45) {
		orientation.x = 1;
	}
	else if (angleX > 135) {
		orientation.x = -1;
	}

	if (angleY < 45) {
		orientation.y = 1;
	}
	else if (angleY > 135) {
		orientation.y = -1;
	}

	if (angleZ < 45) {
		orientation.z = 1;
	}
	else if (angleZ > 135) {
		orientation.z = -1;
	}
	return orientation;
}


/// <summary>
/// Returns the movement that block A needs to make in order to meet with block B
/// </summary>
/// <param name="a">A -> Block that moves</param>
/// <param name="b">B -> Not moving block</param>
/// <returns>glm::vec3 movement</returns>
glm::vec3 MathHelper::objectMeet(Entity * a, Entity * b) {

	glm::vec3 movement = b->absPosition - a->absPosition;
	return movement - getVectorDirection(movement);

}


/// <summary>
/// Distances the point to line.
/// </summary>
/// <param name="point">The point.</param>
/// <param name="lineStart">The line start.</param>
/// <param name="vector">The vector.</param>
/// <returns></returns>
											//    x0				x1					x2
float MathHelper::distancePointToLine(glm::vec3 point, glm::vec3 lineStart, glm::vec3 vector) { //http://mathworld.wolfram.com/Point-LineDistance3-Dimensional.html

	glm::vec3 thirdPoint = lineStart + vector;

	float distance = getVectorLength(glm::cross((thirdPoint - lineStart), (lineStart - point))) / getVectorLength(thirdPoint - lineStart);

	return distance;
}

/// <summary>
/// Returns the vector C->D where D belongs to the line A->B and the distance from C to D is the minimum
/// </summary>
/// <param name="A">A: First point of the line</param>
/// <param name="B">B: Second point of the line</param>
/// <param name="C">C: Point to look for closest</param>
/// <returns></returns>
glm::vec3 MathHelper::shortestVecToLine(glm::vec3 vp1, glm::vec3 vp2, glm::vec3 vl, glm::vec3 pp, glm::vec3 lp) {

	float c; //see notes

	//to get the point only c is needed
	c = (lp.z - pp.z - (lp.x *vp1.z) / vp1.x + (pp.x* vp1.z) / vp1.x + (lp.y* vp1.z* vp2.x) / (-vp1.y *vp2.x + vp1.x *vp2.y) - (pp.y *vp1.z *vp2.x) / (-vp1.y *vp2.x + vp1.x* vp2.y) - (lp.x* vp1.y* vp1.z* vp2.x) / (vp1.x*(-vp1.y *vp2.x + vp1.x* vp2.y)) + (pp.x* vp1.y* vp1.z* vp2.x) / (vp1.x*(-vp1.y *vp2.x + vp1.x* vp2.y)) - (lp.y *vp1.x* vp2.z) / (-vp1.y* vp2.x + vp1.x* vp2.y) + (pp.y* vp1.x* vp2.z) / (-vp1.y* vp2.x + vp1.x* vp2.y) + (lp.x* vp1.y *vp2.z) / (-vp1.y *vp2.x + vp1.x *vp2.y) - (pp.x *vp1.y *vp2.z) / (-vp1.y* vp2.x + vp1.x* vp2.y)) / (-vl.z + (vl.x* vp1.z) / vp1.x - (vl.y* vp1.z* vp2.x) / (-vp1.y* vp2.x + vp1.x *vp2.y) + (vl.x *vp1.y* vp1.z* vp2.x) / (vp1.x*(-vp1.y* vp2.x + vp1.x *vp2.y)) + (vl.y* vp1.x* vp2.z) / (-vp1.y* vp2.x + vp1.x* vp2.y) - (vl.x *vp1.y *vp2.z) / (-vp1.y *vp2.x + vp1.x *vp2.y));
	//b = (lp.y *vp1.x - pp.y* vp1.x + c* vl.y* vp1.x - lp.x* vp1.y + pp.x *vp1.y - c* vl.x *vp1.y) / (-vp1.y* vp2.x + vp1.x* vp2.y);
	//a = (lp.x - pp.x + c *vl.x - b* vp2.x) / vp1.x;

	glm::vec3 Q;

	Q = lp + c * vl;

	return Q;
}

/// <summary>
/// Changes the reference system from the absolute to the given.
/// </summary>
/// <param name="point">The point to change (absolute coordinates).</param>
/// <param name="newOrigin">The new origin.</param>
/// <param name="newX">The new x axis.</param>
/// <param name="newY">The new y axis.</param>
/// <param name="newZ">The new z axis.</param>
/// <returns>A vector containig the changed point</returns>
glm::vec3 MathHelper::changeReferenceSystem(glm::vec3 point, glm::vec3 newOrigin, glm::vec3 newX, glm::vec3 newY, glm::vec3 newZ) {

	point = point - newOrigin;
	glm::vec3 newPoint;

	newPoint.z = (-((newX.z* point.x) / newX.x) - (newX.y* newX.z* newY.x* point.x) / (newX.x*(-newX.y *newY.x + newX.x *newY.y)) + (newX.y *newY.z *point.x) / (-newX.y* newY.x + newX.x* newY.y) + (newX.z *newY.x *point.y) / (-newX.y *newY.x + newX.x *newY.y) - (newX.x *newY.z *point.y) / (-newX.y* newY.x + newX.x *newY.y) + point.z) / (-((newX.z *newZ.x) / newX.x) - (newX.y* newX.z *newY.x *newZ.x) / (newX.x*(-newX.y* newY.x + newX.x *newY.y)) + (newX.y* newY.z *newZ.x) / (-newX.y* newY.x + newX.x *newY.y) + (newX.z* newY.x *newZ.y) / (-newX.y* newY.x + newX.x *newY.y) - (newX.x* newY.z* newZ.y) / (-newX.y *newY.x + newX.x *newY.y) + newZ.z);
	newPoint.y = (newPoint.z* newX.y *newZ.x - newPoint.z* newX.x* newZ.y - newX.y *point.x + newX.x* point.y) / (-newX.y *newY.x + newX.x* newY.y);
	newPoint.x = (-newPoint.y* newY.x - newPoint.z* newZ.x + point.x) / newX.x;

	return newPoint;
}

glm::vec3 MathHelper::rotatePointArroundAxis(glm::vec3 point, glm::vec3 axis, double angle) {

	float v1 = point.x * axis.x + point.y * axis.y + point.z * axis.z;
	double oneMinusCos = 1 - cos(angle);
	double axisModule = sqrt(axis.x * axis.x + axis.y * axis.y + axis.z * axis.z);
	float axisM = axis.x * axis.x + axis.y * axis.y + axis.z * axis.z;

	glm::vec3 rotatedPoint;

	rotatedPoint.x = (float)((axis.x * v1 * oneMinusCos + axisM * point.x * cos(angle) + axisModule * (-axis.z * point.y + axis.y * point.z) * sin(angle)) / axisM);
	rotatedPoint.y = (float)((axis.y * v1 * oneMinusCos + axisM * point.y * cos(angle) + axisModule * (axis.z * point.x - axis.x * point.z) * sin(angle)) / axisM);
	rotatedPoint.z = (float)((axis.z * v1 * oneMinusCos + axisM * point.z * cos(angle) + axisModule * (-axis.y * point.x + axis.x * point.y) * sin(angle)) / axisM);

	return rotatedPoint;
}

/// <summary>
/// Gets the angles from aim.
/// </summary>
/// <param name="viewAxis">The view axis.</param>
/// <returns>"std::pair-pitch, yaw- in radians"</returns>
std::pair<double, double> MathHelper::getAnglesFromAim(glm::vec3 viewAxis) {

	glm::normalize(viewAxis);

	double pitch = atan2(viewAxis.y, sqrt(viewAxis.x * viewAxis.x + viewAxis.z * viewAxis.z));
	double yaw = -atan2(viewAxis.z, viewAxis.x);

	std::pair<double, double> returnValues;
	returnValues.first = pitch;
	returnValues.second = yaw;
	return returnValues;
}

//https://www.maplesoft.com/support/help/Maple/view.aspx?path=MathApps/ProjectionOfVectorOntoPlane
double MathHelper::findRoll(glm::vec3 axisUp, glm::vec3 axisRight) {

	double roll = MathHelper::angleBetween(axisRight, glm::vec3(axisRight.z, 0, axisRight.x));

	if (axisRight.y < 0) {
		roll = 2 * PI - roll;
	}
	return roll;

}


/// <summary>
/// Determines whether the given point is inside the paralelogram formed by parStart and parEnd. Remembrer, this paralelogram vertices are paralel to the world axis (1,0,0)(0,1,0)(0,0,1).
/// </summary>
/// <param name="point">The point.</param>
/// <param name="parStart">Paralelogram start.</param>
/// <param name="parEnd">Paralelogram end.</param>
/// <returns></returns>
bool MathHelper::isInsideParalelogram(glm::vec3 point, glm::vec3 parStart, glm::vec3 parEnd)
{

	glm::vec3 parDiagonal = parEnd - parStart;
	glm::vec3 pointDiagonal = point - parStart;

	bool inside = true;

	if (parDiagonal.x > 0) {
		if (pointDiagonal.x < 0 || pointDiagonal.x > parDiagonal.x) {
			inside = false;
		}
	}
	else {
		if (pointDiagonal.x > 0 || pointDiagonal.x < parDiagonal.x) {
			inside = false;
		}
	}

	if (parDiagonal.y > 0) {
		if (pointDiagonal.y < 0 || pointDiagonal.y > parDiagonal.y) {
			inside = false;
		}
	}
	else {
		if (pointDiagonal.y > 0 || pointDiagonal.y < parDiagonal.y) {
			inside = false;
		}
	}

	if (parDiagonal.z > 0) {
		if (pointDiagonal.z < 0 || pointDiagonal.z > parDiagonal.z) {
			inside = false;
		}
	}
	else {
		if (pointDiagonal.z > 0 || pointDiagonal.z < parDiagonal.z) {
			inside = false;
		}
	}

	return inside;
}


glm::vec3 MathHelper::findVectorPointToParalelogram(glm::vec3 point, glm::vec3 polStart, glm::vec3 polEnd)
{
	glm::vec3 result(0,0,0);

	glm::vec3 polDiagonal = polEnd - polStart;
	glm::vec3 polStartToPoint = point - polStart;

	if (polDiagonal.x > 0) {
		if (polStartToPoint.x < 0) {
			result.x = polStart.x - point.x;
		}
		else if (polStartToPoint.x > polDiagonal.x) {
			result.x = polEnd.x - point.x;
		}
	}
	else {
		if (polStartToPoint.x > 0) {
			result.x = polStart.x - point.x;
		}
		else if (polStartToPoint.x < polDiagonal.x) {
			result.x = polEnd.x - point.x;
		}
	}

	if (polDiagonal.y > 0) {
		if (polStartToPoint.y < 0) {
			result.y = polStart.y - point.y;
		}
		else if (polStartToPoint.y > polDiagonal.y) {
			result.y = polEnd.y - point.y;
		}
	}
	else {
		if (polStartToPoint.y > 0) {
			result.y = polStart.y - point.y;
		}
		else if (polStartToPoint.y < polDiagonal.y) {
			result.y = polEnd.y - point.y;
		}
	}
	
	if (polDiagonal.z > 0) {
		if (polStartToPoint.z < 0) {
			result.z = polStart.z - point.z;
		}
		else if (polStartToPoint.z > polDiagonal.z) {
			result.z = polEnd.z - point.z;
		}
	}
	else {
		if (polStartToPoint.z > 0) {
			result.z = polStart.z - point.z;
		}
		else if (polStartToPoint.z < polDiagonal.z) {
			result.z = polEnd.z - point.z;
		}
	}

	return result;
}