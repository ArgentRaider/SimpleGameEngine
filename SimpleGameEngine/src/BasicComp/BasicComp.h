#pragma once

#ifndef __BASICCOMP_H
#define __BASICCOMP_H

#include <Model/Mesh.h>
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
	static Mesh* drawRect(float a, float b, float c);
	static Mesh* drawSphere(float r);
};





#endif