#pragma once

#ifndef SIMPLE_COLLIDER_H
#define SIMPLE_COLLIDER_H
class Model;

// For now, a collider is simply a rectangle
class Collider
{
public:
	Collider() {};
	Collider(double xmin, double ymin, double zmin, double xmax, double ymax, double zmax, Model* model);
	~Collider() {};
	double xmin, ymin, zmin, xmax, ymax, zmax;
	Model *model;
protected:
	
};

bool ifCollide(const Collider a, const Collider b);

#endif
