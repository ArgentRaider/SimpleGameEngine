#pragma once

#ifndef SIMPLE_COLLIDER_H
#define SIMPLE_COLLIDER_H

#include <Model/Model.h>
// For now, a collider is simply a rectangle
class Collider
{
public:
	Collider() {};
	Collider(double xmin, double ymin, double zmin, double xmax, double ymax, double zmax, Model* model);
	~Collider() {};
	double xmin, ymin, zmin, xmax, ymax, zmax;

protected:
	Model* model;
};

#endif
