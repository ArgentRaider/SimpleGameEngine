#include "Model.h"

#include <iostream>
#include <glm/gtc/matrix_transform.hpp>

void Model::Draw(const Shader& shader) 
{
	for (Mesh mesh : meshes) {
		mesh.Draw(shader);
	}
}

void Model::Translate(glm::vec3 mov)
{
	this->modelMatrix = glm::translate(glm::mat4(), mov) * this->modelMatrix;	// multiply on the left size
}

void Model::Scale(glm::vec3 scale)
{
	this->modelMatrix = glm::scale(glm::mat4(), scale) * this->modelMatrix;

}

void Model::Rotate(float angle, glm::vec3 axis)
{
	// In rotation we multiply the rotate matrix on the right side!
	// Since we always want the model to rotate in its local coordinate system, 
	//	not in the world coordinate system!
	this->modelMatrix = glm::rotate(this->modelMatrix, glm::radians(angle), axis);
	glm::mat4 temp;
	temp = glm::rotate(temp, glm::radians(angle), axis);
	this->Front = glm::normalize(this->Front * temp);
}





Model::~Model()
{
}

void Model::loadModel(std::string path) 
{
	Assimp::Importer importer;
	const aiScene *scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs);
	if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
		std::cout << "ERROR::ASSIMP::" << importer.GetErrorString() << std::endl;
		return;
	}
	directory = path.substr(0, path.find_last_of('/'));
	
	processNode(scene->mRootNode, scene);
}

void Model::processNode(aiNode *node, const aiScene *scene)
{
	// process all meshes (if any) for the node
	for (unsigned int i = 0; i < node->mNumMeshes; i++) {
		aiMesh *mesh = scene->mMeshes[node->mMeshes[i]];
		meshes.push_back(processMesh(mesh, scene));
	}
	// Then process the children nodes
	for (unsigned int i = 0; i < node->mNumChildren; i++) {
		processNode(node->mChildren[i], scene);
	}
}

Mesh Model::processMesh(aiMesh *mesh, const aiScene *scene)
{
	std::vector<Vertex> vertices;
	std::vector<unsigned int> indices;
	std::vector<Texture> textures;
	float shininess = 0.0f;

	for (unsigned int i = 0; i < mesh->mNumVertices; i++) {
		Vertex vertex;
		//... deal with vertex information like pos, normal, texcoord...
		glm::vec3 vector;

		vector.x = mesh->mVertices[i].x;
		vector.y = mesh->mVertices[i].y;
		vector.z = mesh->mVertices[i].z;
		vertex.Position = vector;
		if (first) {
			xmin = xmax = vector.x;
			ymin = ymax = vector.y;
			zmin = zmax = vector.z;
			first = false;
		}
		else {
			if (xmin > vector.x) xmin = vector.x;
			else if (xmax < vector.x) xmax = vector.x;

			if (ymin > vector.y) ymin = vector.y;
			else if (ymax < vector.y) ymax = vector.y;

			if (zmin > vector.z) zmin = vector.z;
			else if (zmax < vector.z) zmax = vector.z;
		}

		vector.x = mesh->mNormals[i].x;
		vector.y = mesh->mNormals[i].y;
		vector.z = mesh->mNormals[i].z;
		vertex.Normal = vector;

		if (mesh->mTextureCoords[0]) {		// If there exists texcoords, we take the first group
			glm::vec2 vec;

			vec.x = mesh->mTextureCoords[0][i].x;
			vec.y = mesh->mTextureCoords[0][i].y;
			vertex.TexCoords = vec;
		}
		else {
			vertex.TexCoords = glm::vec2(0.0f, 0.0f);
		}

		vertices.push_back(vertex);
	}
	// deal with index
	for (unsigned int i = 0; i < mesh->mNumFaces; i++) {
		aiFace face = mesh->mFaces[i];
		for (unsigned int j = 0; j < face.mNumIndices; j++) {
			indices.push_back(face.mIndices[j]);
		}
	}

	// deal with materials
	if (mesh->mMaterialIndex >= 0) {
		aiMaterial *material = scene->mMaterials[mesh->mMaterialIndex];
		material->Get(AI_MATKEY_SHININESS, shininess);

		std::vector<Texture> diffuseMaps = loadMaterialTextures(material, aiTextureType_DIFFUSE, 
																Texture::DiffuseType);
		textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());
		
		std::vector<Texture> specularMaps = loadMaterialTextures(material, aiTextureType_SPECULAR,
																Texture::SpecularType);
		textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());
	}
	
	return Mesh(vertices, indices, textures, shininess);
}

std::vector<Texture> Model::loadMaterialTextures(aiMaterial *mat, aiTextureType type,
													std::string typeName)
{
	std::vector<Texture> textures;
	for (unsigned int i = 0; i < mat->GetTextureCount(type); i++) {
		aiString str;
		mat->GetTexture(type, i, &str);
		bool skip = false;
		for (Texture t : textures_loaded) {
			if (std::strcmp(t.path.c_str(), str.C_Str()) == 0) {
				textures.push_back(t);
				skip = true;
				break;
			}
		}
		if (!skip) {
			Texture texture;
			texture.id = Texture::TextureFromFile(str.C_Str(), directory);
			texture.type = typeName;
			texture.path = str.C_Str();
			textures.push_back(texture);
		}		
	}
	return textures;
}

