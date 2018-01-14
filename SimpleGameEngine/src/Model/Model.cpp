#include "Model.h"

#include <cassert>
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
}

Model::~Model()
{
}

void Model::loadModel(std::string path) 
{
	Assimp::Importer importer;
	const aiScene *scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_CalcTangentSpace);
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

		if (mesh->mNormals) {	// Some obj may not contain normal vectors, causing mesh->mNormals to be null
			vector.x = mesh->mNormals[i].x;
			vector.y = mesh->mNormals[i].y;
			vector.z = mesh->mNormals[i].z;
		}
		else {					// with normal to be 0 vector, the model CANNOT be rendered correctly!!!
			vector = { 0, 0, 0 };
		}
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

		// tangent
		vector.x = mesh->mTangents[i].x;
		vector.y = mesh->mTangents[i].y;
		vector.z = mesh->mTangents[i].z;
		// bitangent
		vector.x = mesh->mBitangents[i].x;
		vector.y = mesh->mBitangents[i].y;
		vector.z = mesh->mBitangents[i].z;

		vertices.push_back(vertex);
	}
	// deal with index
	for (unsigned int i = 0; i < mesh->mNumFaces; i++) {
		aiFace face = mesh->mFaces[i];
#ifndef NDEBUG
		assert(face.mNumIndices == 3);
#endif // !NDEBUG
		for (unsigned int j = 0; j < face.mNumIndices; j++) {
			indices.push_back(face.mIndices[j]);
		}
		// now we can calculate Tangent and Bitangent on our own!!!
		Vertex& p1 = vertices[face.mIndices[0]];
		Vertex& p2 = vertices[face.mIndices[1]];
		Vertex& p3 = vertices[face.mIndices[2]];
		glm::vec3 edge1 = p2.Position - p1.Position;
		glm::vec3 edge2 = p3.Position - p1.Position;
		glm::vec2 deltaUV1 = p2.TexCoords - p1.TexCoords;
		glm::vec2 deltaUV2 = p3.TexCoords - p1.TexCoords;
		float f = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV1.y * deltaUV2.x);
		glm::vec3 tangent;
		tangent.x = f * (deltaUV2.y * edge1.x - deltaUV1.y * edge2.x);
		tangent.y = f * (deltaUV2.y * edge1.y - deltaUV1.y * edge2.y);
		tangent.z = f * (deltaUV2.y * edge1.z - deltaUV1.y * edge2.z);
		glm::vec3 bitangent;
		bitangent.x = f * (-deltaUV2.x * edge1.x + deltaUV1.x * edge2.x);
		bitangent.y = f * (-deltaUV2.x * edge1.y + deltaUV1.x * edge2.y);
		bitangent.z = f * (-deltaUV2.x * edge1.z + deltaUV1.x * edge2.z);
		p1.Tangent = tangent;	p2.Tangent = tangent;	p3.Tangent = tangent;
		p1.Bitangent = bitangent;	p2.Bitangent = bitangent;	p3.Bitangent = bitangent;
	}

	// deal with materials
	if (mesh->mMaterialIndex >= 0) {
		aiMaterial *material = scene->mMaterials[mesh->mMaterialIndex];
		material->Get(AI_MATKEY_SHININESS, shininess);
		shininess /= 10;

		std::vector<Texture> diffuseMaps = loadMaterialTextures(material, aiTextureType_DIFFUSE, 
																Texture::DiffuseType);
		textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());
		
		std::vector<Texture> specularMaps = loadMaterialTextures(material, aiTextureType_SPECULAR,
																Texture::SpecularType);
		textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());

		std::vector<Texture> normalMaps = loadMaterialTextures(material, aiTextureType_HEIGHT,
																Texture::NormalType);
		if (normalMaps.size() == 0) {
			has_normal_map = false;
		}
		else {
			has_normal_map = true;
			textures.insert(textures.end(), normalMaps.begin(), normalMaps.end());
		}
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