#pragma once

#ifndef SIMPLE_MODEL_H
#define SIMPLE_MODEL_H

#include <string>
#include <vector>
#include <assimp\Importer.hpp>
#include <assimp\scene.h>
#include <assimp\postprocess.h>
#include <glm/glm.hpp>
#include <Collider/Collider.h>
#include <ShaderProgram/Shader.h>

#include "Mesh.h"

class Model
{
public:
	glm::vec4 Front;
	std::vector<Mesh> meshes;
	glm::mat4 modelMatrix;
	glm::vec3 shift;
	

	Model(std::vector<Mesh> meshes, bool hasNormalMap = false) :meshes(meshes), has_normal_map(hasNormalMap) { }
	Model (){}

	Model(char *path) {
		loadModel(path);
		Front = { 0.0, 0.0, -1.0, 1.0 };
		shift = { 0.0, 0.0, 0.0 };
		this->collider = Collider(xmin, ymin, zmin, xmax, ymax, zmax, this);
	}
	Model(std::string path) {
		loadModel(path);
		Front = { 0.0, 0.0, -1.0, 1.0 };
		shift = { 0.0, 0.0, 0.0 };
		this->collider = Collider(xmin, ymin, zmin, xmax, ymax, zmax, this);
	}

	// You may overwrite these functions
	virtual void Draw(const Shader& shader);								//Simply draw the model at the intitial postion. 
														//You should do transformation on yourself before calling Draw()
	virtual void Translate(glm::vec3 mov);
	virtual void Scale(glm::vec3 scale);
	virtual void Rotate(float radians, glm::vec3 axis);


	glm::mat4 getModelMatrix() const{ return this->modelMatrix; }
	// Maybe useful in collision detection.
	const Collider& getCollider() const { return this->collider; }

	float rot = 0.0f;

	void setShift(float x, float y, float z) { this->shift = glm::vec3(x,y,z); }

	~Model();

	
	bool isActive() { return this->active; }
	void setActive(bool b) { this->active = b; }
	bool hasNormalMap() { return this->has_normal_map; }
	
protected:

	std::string directory;
	std::vector<Texture> textures_loaded; // To avoid reloading the same texture for multiple times.
										  // Actually we only need Texture.path to use here.
	
	bool has_normal_map;

	Collider collider;
	
	bool active = true;

	double xmin, ymin, zmin, xmax, ymax, zmax;


	void loadModel(std::string path);
	void processNode(aiNode *node, const aiScene *scene);
	Mesh processMesh(aiMesh *mesh, const aiScene *scene);
	std::vector<Texture> loadMaterialTextures(aiMaterial *mat, aiTextureType type,
		std::string typeName);



	bool first = true;					// simply for calculating collider
};
#endif


