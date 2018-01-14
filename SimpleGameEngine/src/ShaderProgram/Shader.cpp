#include "Shader.h"

#include <glm\glm.hpp>
#include <glm\gtc\type_ptr.hpp>

Shader::Shader(const GLchar * vertexPath, const GLchar * fragmentPath):vertexPath(vertexPath), fragmentPath(fragmentPath)
{}
void Shader::init(){
	// 1. Retrieve the source codes of both shaders from the file path
	std::string vertexCode, fragmentCode;
	std::ifstream vertexFile, fragmentFile;
	// set the exception mask to ensure the ifstream objects can throw exceptions
	vertexFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
	fragmentFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
	try
	{
		// open files
		vertexFile.open(vertexPath);
		fragmentFile.open(fragmentPath);
		std::stringstream vertexStream, fragmentStream;
		// read files' buffer contents into streams
		vertexStream << vertexFile.rdbuf();
		fragmentStream << fragmentFile.rdbuf();
		// close file handlers
		vertexFile.close();
		fragmentFile.close();
		// convert the string stream into string
		vertexCode = vertexStream.str();
		fragmentCode = fragmentStream.str();
	}
	catch (const std::ifstream::failure e)
	{
		std::cout << "ERROR::SHADER::FILE_READ_FAILED" << std::endl;
	}
	const char* vShaderCode = vertexCode.c_str();
	const char* fShaderCode = fragmentCode.c_str();

	// 2. compile and link the shaders
	unsigned int vertex, fragment;
	int success;
	char infoLog[512];

	// vertex shader
	vertex = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertex, 1, &vShaderCode, NULL);
	glCompileShader(vertex);
	// print compile errors if any
	glGetShaderiv(vertex, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(vertex, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
	}

	// fragment shader
	fragment = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragment, 1, &fShaderCode, NULL);
	glCompileShader(fragment);
	glGetShaderiv(fragment, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(fragment, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
	}

	// shader program
	ID = glCreateProgram();
	glAttachShader(ID, vertex);
	glAttachShader(ID, fragment);
	glLinkProgram(ID);
	// print linking errors if any
	glGetProgramiv(ID, GL_LINK_STATUS, &success);
	if (!success)
	{
		glGetProgramInfoLog(ID, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
	}

	// delete the shaders as they are no longer needed
	glDeleteShader(vertex);
	glDeleteShader(fragment);
}


Shader::~Shader()
{
}

void Shader::use() const
{
	glUseProgram(ID);
}

void Shader::setBool(const std::string & name, bool value) const
{
	glUniform1i(glGetUniformLocation(ID, name.c_str()), (int)value);
}
void Shader::setInt(const std::string & name, int value) const
{
	glUniform1i(glGetUniformLocation(ID, name.c_str()), value);
}
void Shader::setFloat(const std::string & name, float value) const
{
	glUniform1f(glGetUniformLocation(ID, name.c_str()), value);
}

void Shader::setMat4(const std::string & name, glm::mat4 matrix) const
{
	glUniformMatrix4fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, glm::value_ptr(matrix));
}

void Shader::setMat4(const std::string & name, float * value_ptr) const
{
	glUniformMatrix4fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, value_ptr);
}

void Shader::setVec3(const std::string & name, glm::vec3 vector) const
{
	glUniform3fv(glGetUniformLocation(ID, name.c_str()), 1, glm::value_ptr(vector));
}

void Shader::setVec3(const std::string &name, float* value_ptr)const
{
	glUniform3fv(glGetUniformLocation(ID, name.c_str()), 1, value_ptr);
}

void Shader::setVec3(const std::string & name, float x, float y, float z) const
{
	glUniform3f(glGetUniformLocation(ID, name.c_str()), x, y, z);
}

void Shader::setVec4(const std::string & name, glm::vec4 vector) const
{
	glUniform4fv(glGetUniformLocation(ID, name.c_str()), 1, glm::value_ptr(vector));
}

void Shader::setVec4(const std::string &name, float* value_ptr)const
{
	glUniform4fv(glGetUniformLocation(ID, name.c_str()), 1, value_ptr);
}

void Shader::setVec4(const std::string & name, float x, float y, float z, float w) const
{
	glUniform4f(glGetUniformLocation(ID, name.c_str()), x, y, z, w);
}

void LightShader::setDirLight(int index, glm::vec3 direction, glm::vec3 ambient, glm::vec3 diffuse, glm::vec3 specular)
{
	use();
	if (index < dirNum) {
		std::stringstream ss;
		std::string identifier;
		ss << "dirLights[" << index << "].direction";	ss >> identifier;
		setVec3(identifier, direction);	ss.clear();
		ss << "dirLights[" << index << "].ambient";		ss >> identifier;
		setVec3(identifier, ambient);		ss.clear();
		ss << "dirLights[" << index << "].diffuse";		ss >> identifier;
		setVec3(identifier, diffuse);		ss.clear();
		ss << "dirLights[" << index << "].specular";	ss >> identifier;
		setVec3(identifier, specular);	ss.clear();
	}
}

void LightShader::addDirLight(glm::vec3 direction, glm::vec3 ambient, glm::vec3 diffuse, glm::vec3 specular)
{
	setDirLight(dirNum++, direction, ambient, diffuse, specular);
	setInt("NR_DIR_LIGHTS", dirNum);
	
}

void LightShader::setPointLight(int index, glm::vec3 position, glm::vec3 ambient, glm::vec3 diffuse, glm::vec3 specular,
	float constant, float linear, float quadratic)
{
	use();
	if (index < pointNum) {
		std::stringstream ss;
		std::string identifier;
		ss << "pointLights[" << index << "].position";		ss >> identifier;
		setVec3(identifier, position);		ss.clear();
		ss << "pointLights[" << index << "].ambient";		ss >> identifier;
		setVec3(identifier, ambient);			ss.clear();		
		ss << "pointLights[" << index << "].diffuse";		ss >> identifier;
		setVec3(identifier, diffuse);			ss.clear();
		ss << "pointLights[" << index << "].specular";		ss >> identifier;
		setVec3(identifier, specular);		ss.clear();
		ss << "pointLights[" << index << "].constant";		ss >> identifier;
		setFloat(identifier, constant);		ss.clear();
		ss << "pointLights[" << index << "].linear";		ss >> identifier;
		setFloat(identifier, linear);		ss.clear();
		ss << "pointLights[" << index << "].quadratic";		ss >> identifier;
		setFloat(identifier, quadratic);	ss.clear();
	}
}

void LightShader::addPointLight(glm::vec3 position, glm::vec3 ambient, glm::vec3 diffuse, glm::vec3 specular, float constant, float linear, float quadratic)
{
	setPointLight(pointNum++, position, ambient, diffuse, specular, constant, linear, quadratic);
	setInt("NR_POINT_LIGHTS", pointNum);
	
}

void LightShader::setSpotLight(int index, glm::vec3 position, glm::vec3 direction, glm::vec3 diffuse, glm::vec3 specular,
	float constant, float linear, float quadratic, float cutoff, float outerCutoff)
{
	use();
	if (index < pointNum) {
		std::stringstream ss;
		std::string identifier;
		ss << "pointLights[" << index << "].position";		ss >> identifier;
		setVec3(identifier, position);		ss.clear();
		ss << "pointLights[" << index << "].direction";		ss >> identifier;
		setVec3(identifier, direction);		ss.clear();
		ss << "pointLights[" << index << "].diffuse";		ss >> identifier;
		setVec3(identifier, diffuse);			ss.clear();
		ss << "pointLights[" << index << "].specular";		ss >> identifier;
		setVec3(identifier, specular);		ss.clear();	
		ss << "pointLights[" << index << "].constant";		ss >> identifier;
		setFloat(identifier, constant);		ss.clear();
		ss << "pointLights[" << index << "].linear";		ss >> identifier;
		setFloat(identifier, linear);		ss.clear();
		ss << "pointLights[" << index << "].quadratic";		ss >> identifier;
		setFloat(identifier, quadratic);		ss.clear();
		ss << "pointLights[" << index << "].cutoff";		ss >> identifier;
		setFloat(identifier, cutoff);			ss.clear();
		ss << "pointLights[" << index << "].outerCutoff";	ss >> identifier;
		setFloat(identifier, outerCutoff);	ss.clear();
	}
}

void LightShader::addSpotLight(glm::vec3 position, glm::vec3 direction, glm::vec3 diffuse, glm::vec3 specular, float constant, float linear, float quadratic, float cutoff, float outerCutoff)
{
	setSpotLight(spotNum++, position, direction, diffuse, specular, constant,
		linear, quadratic, cutoff, outerCutoff);
	setInt("NR_SPOT_LIGHTS", spotNum);
}
