#include "Tank.h"
#include <glm/gtc/matrix_transform.hpp>

void Tank::Translate(glm::vec3 mov)
{
	this->modelMatrix = glm::translate(glm::mat4(), mov) * this->modelMatrix;	// multiply on the left size
	if (nullptr != barrel) {
		barrel->modelMatrix = glm::translate(glm::mat4(), mov) * barrel->modelMatrix;
	}
}

void Tank::Scale(glm::vec3 scale)
{
	this->modelMatrix = glm::scale(glm::mat4(), scale) * this->modelMatrix;
	if (nullptr != barrel) {
		barrel->modelMatrix = glm::scale(glm::mat4(), scale) * barrel->modelMatrix;
	}
}

void Tank::Rotate(float angle, glm::vec3 axis, int type, int adapt)
{
	// In rotation we multiply the rotate matrix on the right side!
	// Since we always want the model to rotate in its local coordinate system, 
	//	not in the world coordinate system!
	//modelMatrix_temp = modelMatrix;
	float radian = 0.0;
	if (type == RADIAN) {
		radian = angle;
	}
	else
	{
		radian = glm::radians(angle);
	}
	this->modelMatrix = glm::rotate(this->modelMatrix, radian, axis);
	glm::mat4 temp;
	temp = glm::rotate(temp, radian, axis);
	if (adapt == UNADAPT)
		this->Front = glm::normalize(this->Front * temp);

	printf("FRONT:   %lf  %lf  %lf  %lf\n", Front.x, Front.y, Front.z, Front.w);

	if (nullptr != barrel) {
		barrel->modelMatrix = glm::rotate(barrel->modelMatrix, radian, axis);
		if (adapt == UNADAPT)
			barrel->Front = glm::normalize(barrel->Front * temp);
	}
}

void Tank::adjustBarrelUp() {
	if (nullptr != barrel) {
		barrel->Rotate(0.1, glm::vec3(1.0f, 0.0f, 0.0f), ANGLE, UNADAPT);
		barrel->rot += 0.1;
	}
}

void Tank::adjustBarrelDown() {
	if (nullptr != barrel) {
		barrel->Rotate(-0.1, glm::vec3(1.0f, 0.0f, 0.0f), ANGLE, UNADAPT);
		barrel->rot -= 0.1;
	}
}

void Tank::backout() {
	if (nullptr != barrel) {
		barrel->Rotate(-barrel->rot, glm::vec3(1.0f, 0.0f, 0.0f), ANGLE, UNADAPT);
		barrel->rot = 0.0f;
	}
}

void Tank::addBarrel(Model *bar) {
	barrel = bar;
}