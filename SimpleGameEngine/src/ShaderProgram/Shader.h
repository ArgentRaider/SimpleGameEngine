#pragma once

#ifndef SIMPLE_SHADER_H
#define SIMPLE_SHADER_H

#include <glad\glad.h>

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>
#include <glm\glm.hpp>
#include <glm\gtc\type_ptr.hpp>

class Shader
{	
public:
	// the program ID
	unsigned int ID;

	// Constructors taking the path of the shader files as arguments
	Shader(const GLchar* vertexPath, const GLchar* fragmentPath);

	~Shader();

	// use/activate the shader
	void init();
	void use() const;
	// utility uniform functions
	void setBool(const std::string &name, bool value)const;
	void setInt(const std::string &name, int value)const;
	void setFloat(const std::string &name, float value)const;
	void setMat4(const std::string &name, glm::mat4 matrix)const;
	void setMat4(const std::string &name, float* value_ptr)const;
	void setVec3(const std::string &name, glm::vec3 vector)const;
	void setVec3(const std::string &name, float* value_ptr)const;
	void setVec3(const std::string &name, float x, float y, float z)const;
	void setVec4(const std::string &name, glm::vec4 vector)const;
	void setVec4(const std::string &name, float* value_ptr)const;
	void setVec4(const std::string &name, float x, float y, float z, float w)const;
	const GLchar *vertexPath, *fragmentPath;
};

class LightShader : public Shader
{
public:
	// light number
	int dirNum = 0, pointNum = 0, spotNum = 0;

	LightShader(int dirNum = 0, int pointNum = 0, int spotNum = 0) : dirNum(dirNum), pointNum(pointNum), spotNum(spotNum),
		Shader("src/shaders/lightVertexShader.vertex", "src/shaders/lightFragmentShader.frag")
	{}

	void init() {
		Shader::init();
		use();
		setInt("NR_DIR_LIGHTS", dirNum);
		setInt("NR_POINT_LIGHTS", pointNum);
		setInt("NR_SPOT_LIGHTS", spotNum);
	}

	void setDirLight(int index, glm::vec3 direction, glm::vec3 ambient, glm::vec3 diffuse, glm::vec3 specular);
	void addDirLight(glm::vec3 direction, glm::vec3 ambient, glm::vec3 diffuse, glm::vec3 specular);
	void setPointLight(int index, glm::vec3 position, glm::vec3 ambient, glm::vec3 diffuse, glm::vec3 specular,
						float constant, float linear, float quadratic);
	void addPointLight(glm::vec3 position, glm::vec3 ambient, glm::vec3 diffuse, glm::vec3 specular,
						float constant, float linear, float quadratic);
	void setSpotLight(int index, glm::vec3 position, glm::vec3 direction, glm::vec3 diffuse, glm::vec3 specular,
						float constant, float linear, float quadratic, float cutoff, float outerCutoff);
	void addSpotLight(glm::vec3 position, glm::vec3 direction, glm::vec3 diffuse, glm::vec3 specular,
						float constant, float linear, float quadratic, float cutoff, float outerCutoff);
};

class SkyboxShader : public Shader {
public:
	SkyboxShader():Shader("src/shaders/skyboxShader.vertex", "src/shaders/skyboxShader.frag"){}
};

class CharacterShader : public Shader {
public:
	CharacterShader() :Shader("src/shaders/characterVertexShader.vertex", "src/shaders/characterFragmentShader.frag") {}
};

class TwoDShader : public Shader {
public:
	TwoDShader() :Shader("src/shaders/2DVertexShader.vertex", "src/shaders/2DFragmentShader.frag") {}
};

class BillboardShader : public Shader {
public:
	BillboardShader() :Shader("src/shaders/billboardShader.vertex", "src/shaders/billboardShader.frag"), geoPath("src/shaders/billboardShader.geo") {}
	void init();
	const GLchar* geoPath;
};
#endif