#pragma once

#ifndef __BASICCOMP_H
#define __BASICCOMP_H

#include <Model/Model.h>
#include <vector>

class BasicComp {
private:
	static std::vector<Vertex> vertices;
	static std::vector<unsigned int> indices;
	static std::vector<Texture> textures;
	static float shininess;

	static void setTexture();
	static void clear();

public:
	// remember to free them on your own !!!
	static Model* drawRect(float a, float b, float c);
	static Model* drawSphere(float r);
};





#endif