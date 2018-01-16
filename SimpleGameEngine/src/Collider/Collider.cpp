#include "Collider.h"
#include <glm/glm.hpp>
#include <Model/Model.h>

Collider::Collider(double xmin, double ymin, double zmin, double xmax, double ymax, double zmax, Model* model)
	:xmin(xmin), ymin(ymin), zmin(zmin), xmax(xmax), ymax(ymax), zmax(zmax), model(model)
{}

bool collideOnAxis(glm::vec3 points[8], float boundMin, float boundMax, glm::vec3 axis);

bool ifCollide(const Collider a, const Collider b)
{
	glm::mat4 aModel = a.model->getModelMatrix(), bModel = b.model->getModelMatrix();
	glm::vec3 aPoints[8] = { glm::vec3(a.xmin, a.ymin, a.zmin), glm::vec3(a.xmin, a.ymin, a.zmax), glm::vec3(a.xmin, a.ymax, a.zmin), glm::vec3(a.xmin, a.ymax, a.zmax),
		glm::vec3(a.xmax, a.ymin, a.zmin), glm::vec3(a.xmax, a.ymin, a.zmax), glm::vec3(a.xmax, a.ymax, a.zmin), glm::vec3(a.xmax, a.ymax, a.zmax) };
	glm::vec3 bPoints[8] = { glm::vec3(b.xmin, b.ymin, b.zmin), glm::vec3(b.xmin, b.ymin, b.zmax), glm::vec3(b.xmin, b.ymax, b.zmin), glm::vec3(b.xmin, b.ymax, b.zmax),
		glm::vec3(b.xmax, b.ymin, b.zmin), glm::vec3(b.xmax, b.ymin, b.zmax), glm::vec3(b.xmax, b.ymax, b.zmin), glm::vec3(b.xmax, b.ymax, b.zmax) };
	
	for (int i = 0; i < 8; i++) {
		aPoints[i] = glm::vec3(aModel * glm::vec4(aPoints[i], 1));
		bPoints[i] = glm::vec3(bModel * glm::vec4(bPoints[i], 1));
	}

	int compareRes = 0;
	// axis of b
	glm::vec3 xAxis, yAxis, zAxis;
	xAxis = glm::normalize(glm::vec3(bModel * glm::vec4(1, 0, 0, 0) ));
	yAxis = glm::normalize(glm::vec3(bModel * glm::vec4(0, 1, 0, 0)));
	zAxis = glm::normalize(glm::vec3(bModel * glm::vec4(0, 0, 1, 0)));
	float
		xmin = glm::dot(glm::vec3(bModel * glm::vec4(b.xmin, 0, 0, 1)), xAxis),
		xmax = glm::dot(glm::vec3(bModel * glm::vec4(b.xmax, 0, 0, 1)), xAxis),
		ymin = glm::dot(glm::vec3(bModel * glm::vec4(0, b.ymin, 0, 1)), yAxis),
		ymax = glm::dot(glm::vec3(bModel * glm::vec4(0, b.ymax, 0, 1)), yAxis),
		zmin = glm::dot(glm::vec3(bModel * glm::vec4(0, 0, b.zmin, 1)), zAxis),
		zmax = glm::dot(glm::vec3(bModel * glm::vec4(0, 0, b.zmax, 1)), zAxis);
	if (!collideOnAxis(aPoints, xmin, xmax, xAxis))
		return false;
	if (!collideOnAxis(aPoints, ymin, ymax, yAxis))
		return false;
	if (!collideOnAxis(aPoints, zmin, zmax, zAxis))
		return false;

	// axis of a, same procedure as above
	xAxis = glm::normalize(glm::vec3(aModel * glm::vec4(1, 0, 0, 0)));
	yAxis = glm::normalize(glm::vec3(aModel * glm::vec4(0, 1, 0, 0)));
	zAxis = glm::normalize(glm::vec3(aModel * glm::vec4(0, 0, 1, 0)));
	xmin = glm::dot(glm::vec3(aModel * glm::vec4(a.xmin, 0, 0, 1)), xAxis);
	xmax = glm::dot(glm::vec3(aModel * glm::vec4(a.xmax, 0, 0, 1)), xAxis);
	ymin = glm::dot(glm::vec3(aModel * glm::vec4(0, a.ymin, 0, 1)), yAxis);
	ymax = glm::dot(glm::vec3(aModel * glm::vec4(0, a.ymax, 0, 1)), yAxis);
	zmin = glm::dot(glm::vec3(aModel * glm::vec4(0, 0, a.zmin, 1)), zAxis);
	zmax = glm::dot(glm::vec3(aModel * glm::vec4(0, 0, a.zmax, 1)), zAxis);
	if (!collideOnAxis(bPoints, xmin, xmax, xAxis))
		return false;
	if (!collideOnAxis(bPoints, ymin, ymax, yAxis))
		return false;
	if (!collideOnAxis(bPoints, zmin, zmax, zAxis))
		return false;
	return true;
}

bool collideOnAxis(glm::vec3 points[8], float boundMin, float boundMax, glm::vec3 axis)
{
	if (boundMin > boundMax) {
		std::swap(boundMax, boundMin);
	}
	int res = 0;
	for (int i = 0; i < 8; i++) {
		float dot = glm::dot(points[i], axis);
		if (dot >= boundMin && dot <= boundMax)
			return true;

		if (!res) {
			res = dot < boundMin ? -1 : 1;
		}
		else if (res < 0 && dot > boundMax || res > 0 && dot < boundMin) {
			return true;
		}
	}
	return false;
}
