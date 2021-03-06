#ifndef SIMPLE_TANK_H
#define SIMPLE_TANK_H

#include "../Model.h"

class Tank : public Model {
public:
	Tank(char *path) :Model(path) {}
	Tank(std::string path) :Model(path) {}

	virtual void Translate(glm::vec3 mov);
	virtual void Scale(glm::vec3 scale);
	virtual void Rotate(float radians, glm::vec3 axis, int type, int adapt);

	void addBarrel(Model *bar);

	void adjustBarrelUp();

	void adjustBarrelDown();

	void backout();
	Model *barrel = nullptr;
};

#endif 
