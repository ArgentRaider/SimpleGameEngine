#include "BasicComp.h"

std::vector<Vertex> BasicComp::vertices=std::vector<Vertex>();
std::vector<unsigned int> BasicComp::indices = std::vector<unsigned int>();
std::vector<Texture> BasicComp::textures = std::vector<Texture>();
float BasicComp::shininess=32.0f;

void BasicComp::setTexture()
{
	unsigned int diffuseMap = Texture::TextureFromFile("container2.png", "res/textures/");
	unsigned int specularMap = Texture::TextureFromFile("container2_specular.png", "res/textures/");
	Texture diff; diff.id = diffuseMap; diff.type = Texture::DiffuseType;
	Texture spec; spec.id = specularMap; spec.type = Texture::SpecularType;
	textures.push_back(diff);
	textures.push_back(spec);
}

void BasicComp::clear()
{
	vertices.clear();
	indices.clear();
	textures.clear();
	shininess = 32.0f;
}

Mesh* BasicComp::drawRect(float a, float b, float c)
{
	// front
	vertices.push_back(Vertex(glm::vec3(-a / 2, c / 2, b / 2), glm::vec3(0, 0, 1), glm::vec2(-a / 2, c / 2)));
	vertices.push_back(Vertex(glm::vec3(-a / 2, -c / 2, b / 2), glm::vec3(0, 0, 1), glm::vec2(-a / 2, -c / 2)));
	vertices.push_back(Vertex(glm::vec3(a / 2, c / 2, b / 2), glm::vec3(0, 0, 1), glm::vec2(a / 2, c / 2)));
	vertices.push_back(Vertex(glm::vec3(a / 2, -c / 2, b / 2), glm::vec3(0, 0, 1), glm::vec2(a / 2, -c / 2)));
	// right
	vertices.push_back(Vertex(glm::vec3(a / 2, c / 2, b / 2), glm::vec3(1, 0, 0), glm::vec2(c / 2, b / 2)));
	vertices.push_back(Vertex(glm::vec3(a / 2, -c / 2, b / 2), glm::vec3(1, 0, 0), glm::vec2(-c / 2, b / 2)));
	vertices.push_back(Vertex(glm::vec3(a / 2, c / 2, -b / 2), glm::vec3(1, 0, 0), glm::vec2(c / 2, -b / 2)));
	vertices.push_back(Vertex(glm::vec3(a / 2, -c / 2, -b / 2), glm::vec3(1, 0, 0), glm::vec2(-c / 2, -b / 2)));
	// back
	vertices.push_back(Vertex(glm::vec3(-a / 2, c / 2, -b / 2), glm::vec3(0, 0, -1), glm::vec2(-a / 2, c / 2)));
	vertices.push_back(Vertex(glm::vec3(-a / 2, -c / 2, -b / 2), glm::vec3(0, 0, -1), glm::vec2(-a / 2, -c / 2)));
	vertices.push_back(Vertex(glm::vec3(a / 2, c / 2, -b / 2), glm::vec3(0, 0, -1), glm::vec2(a / 2, c / 2)));
	vertices.push_back(Vertex(glm::vec3(a / 2, -c / 2, -b / 2), glm::vec3(0, 0, -1), glm::vec2(a / 2, -c / 2)));
	// left
	vertices.push_back(Vertex(glm::vec3(-a / 2, c / 2, b / 2), glm::vec3(-1, 0, 0), glm::vec2(c / 2, b / 2)));
	vertices.push_back(Vertex(glm::vec3(-a / 2, -c / 2, b / 2), glm::vec3(-1, 0, 0), glm::vec2(-c / 2, b / 2)));
	vertices.push_back(Vertex(glm::vec3(-a / 2, c / 2, -b / 2), glm::vec3(-1, 0, 0), glm::vec2(c / 2, -b / 2)));
	vertices.push_back(Vertex(glm::vec3(-a / 2, -c / 2, -b / 2), glm::vec3(-1, 0, 0), glm::vec2(-c / 2, -b / 2)));
	// top
	vertices.push_back(Vertex(glm::vec3(-a / 2, c / 2, -b / 2), glm::vec3(0, 1, 0), glm::vec2(-a / 2, -b / 2)));
	vertices.push_back(Vertex(glm::vec3(-a / 2, c / 2, b / 2), glm::vec3(0, 1, 0), glm::vec2(-a / 2, b / 2)));
	vertices.push_back(Vertex(glm::vec3(a / 2, c / 2, -b / 2), glm::vec3(0, 1, 0), glm::vec2(a / 2, -b / 2)));
	vertices.push_back(Vertex(glm::vec3(a / 2, c / 2, b / 2), glm::vec3(0, 1, 0), glm::vec2(a / 2, b / 2)));
	// bottom
	vertices.push_back(Vertex(glm::vec3(-a / 2, -c / 2, -b / 2), glm::vec3(0, -1, 0), glm::vec2(-a / 2, -b / 2)));
	vertices.push_back(Vertex(glm::vec3(-a / 2, -c / 2, b / 2), glm::vec3(0, -1, 0), glm::vec2(-a / 2, b / 2)));
	vertices.push_back(Vertex(glm::vec3(a / 2, -c / 2, -b / 2), glm::vec3(0, -1, 0), glm::vec2(a / 2, -b / 2)));
	vertices.push_back(Vertex(glm::vec3(a / 2, -c / 2, b / 2), glm::vec3(0, -1, 0), glm::vec2(a / 2, b / 2)));

	for (int i = 0; i < 36; i += 4)
	{
		indices.push_back(i);
		indices.push_back(i+1);
		indices.push_back(i+3);
		indices.push_back(i);
		indices.push_back(i+3);
		indices.push_back(i+2);
	}
	setTexture();
	Mesh* mesh = new Mesh(vertices, indices, textures, shininess);
	clear();
	return mesh;
}

Mesh* BasicComp::drawSphere(float r)
{
	float angleA, angleB;
	float step=2.0f;
	int curV = -1;
	for (angleA = 90; angleA > -90; angleA -= step)
	{
		int n = 0;
		for (angleB = 0; angleB <= 360; angleB += step)
		{
			// x=r*cosa*sinb
			// y=r*sina
			// z=r*cosa*cosb
			float a0 = glm::radians(angleA);
			float b = glm::radians(angleB);
			float a1 = glm::radians(angleA - step);
			float x0 = r*cos(a0)*sin(b);
			float y0 = r*sin(a0);
			float z0 = r*cos(a0)*cos(b);
			float x1 = r*cos(a1)*sin(b);
			float y1 = r*sin(a1);
			float z1 = r*cos(a1)*cos(b);
			vertices.push_back(Vertex(glm::vec3(x0, y0, z0), glm::vec3(x0, y0, z0), glm::vec2(x0, y0)));
			vertices.push_back(Vertex(glm::vec3(x1, y1, z1), glm::vec3(x1, y1, z1), glm::vec2(x1, y1)));
			n++;
			curV += 2;
			if (n == 2)
			{
				indices.push_back(curV - 3);
				indices.push_back(curV - 2);
				indices.push_back(curV);
				indices.push_back(curV - 3);
				indices.push_back(curV);
				indices.push_back(curV - 1);
				n = 1;
			}
		}
	}
	setTexture();
	Mesh* mesh = new Mesh(vertices, indices, textures, shininess);
	clear();
	return mesh;
}