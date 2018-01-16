#ifndef CANNON_BALL_H
#define CANNON_BALL_H
#include <Model/Model.h>
class Cannonball {
public:
	Cannonball();
	void init();
	void set(Model* model, glm::vec3 v0, glm::vec3 pos0);
	void Render();

	Model* model;
	glm::vec3 v;
	glm::vec3 pos;
};

#endif