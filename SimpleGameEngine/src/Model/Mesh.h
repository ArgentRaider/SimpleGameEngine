#pragma once

#ifndef SIMPLE_MESH_H
#define SIMPLE_MESH_H

#include <vector>
#include <string>
#include <glm\glm.hpp>
#include <ShaderProgram/Shader.h>

struct Vertex {
	glm::vec3 Position;
	glm::vec3 Normal;
	glm::vec2 TexCoords;
	glm::vec3 Tangent;
	glm::vec3 Bitangent;
	Vertex(){}
	Vertex(glm::vec3 pos, glm::vec3 norm, glm::vec2 texcoord):Position(pos),Normal(norm),TexCoords(texcoord){}
};

struct Texture {
	static const std::string DiffuseType;
	static const std::string SpecularType; 
	static const std::string NormalType;
	unsigned int id;
	std::string type = std::string();
	std::string path = std::string();

	static unsigned int TextureFromFile(const char *path, const std::string &directory, bool gamma = false);
};

class Mesh {
public:
	std::vector<Vertex> vertices;
	std::vector<unsigned int> indices;
	std::vector<Texture> textures;
	float shininess;

	Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices, std::vector<Texture> textures, float shininess = 0.0f);
	void Draw(const Shader& shader);
protected:
	unsigned int VAO, VBO, EBO;
	
	void setupMesh();
};

#endif
