#include "RenderEngine.h"
#include <stb_image.h>
#include <iostream>

// Static variable list
float RenderEngine::SCR_WIDTH = 800;
float RenderEngine::SCR_HEIGHT = 600;
float RenderEngine::bgR = 0.0f, 
	  RenderEngine::bgG = 0.0f, 
	  RenderEngine::bgB = 0.0f;

float RenderEngine::deltaTime = 0, RenderEngine::lastFrame = 0;
GLFWwindow* RenderEngine::window = nullptr;
GameLogic*	RenderEngine::currentLogic = nullptr;
std::string RenderEngine::title = "Simple Game Engine";

unsigned int RenderEngine::sky(0), 
			 RenderEngine::skyVAO(0), 
			 RenderEngine::skyVBO(0);
bool RenderEngine::haveSky = false;
bool RenderEngine::useNormalMap = false;
SkyboxShader RenderEngine::skyboxShader;
std::vector<Vertex> RenderEngine::terrainVertices = std::vector<Vertex>();
LightShader RenderEngine::defaultShader(0, 0, 0);
CharacterShader RenderEngine::characterShader;
TwoDShader RenderEngine::twoDShader;

Camera* RenderEngine::camera = nullptr;
std::vector<ModelAndShader> RenderEngine::models;


void _default_framebuffer_size_callback(GLFWwindow * window, int width, int height);

bool RenderEngine::init(std::string title)
{
	// glfw: initialize and configure
	// ------------------------------
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	// glfw: Window Object creation
	window = glfwCreateWindow((int)SCR_WIDTH, (int)SCR_HEIGHT, title.c_str(), NULL, NULL);
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW Window" << std::endl;
		glfwTerminate();
		return false;
	}
	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, FramebufferSizeCallback); // Register a callback function for resizing a window.
	glfwSetCursorPosCallback(window, MouseCallback);
	glfwSetScrollCallback(window, ScrollCallback);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		std::cout << "Failed to initialize GLAD" << std::endl;
		glfwTerminate();
		return false;
	}
	glEnable(GL_DEPTH_TEST);

	RenderEngine::title = title;
	RenderEngine::skyboxShader.init();
	RenderEngine::defaultShader.init();
	RenderEngine::characterShader.init();
	RenderEngine::twoDShader.init();
	return true;
}

void RenderEngine::run()
{
	// render loop
	// -----------
	while (!glfwWindowShouldClose(window)) {
		// per-frame time logic
		float currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		// input
		ProcessInput(window, deltaTime);

		glm::mat4 projection = glm::perspective(glm::radians(camera->Zoom), RenderEngine::SCR_WIDTH / RenderEngine::SCR_HEIGHT, 0.1f, 500.0f);
		glm::mat4 view = camera->GetViewMatrix();

		// Draw background
		glClearColor(bgR, bgG, bgB, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		// Draw sky
		if (haveSky) {
			glDepthMask(GL_FALSE);
			skyboxShader.use();
			skyboxShader.setInt("skybox", 0);
			skyboxShader.setMat4("view", glm::mat4(glm::mat3((view))));
			skyboxShader.setMat4("projection", projection);
			glBindVertexArray(skyVAO);
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_CUBE_MAP, sky);
			glDrawArrays(GL_TRIANGLES, 0, 36);
			glBindVertexArray(0);
			glDepthMask(GL_TRUE);
		}

		// render
		// Draw models
		for (ModelAndShader m : models) {
			Model* mod = m.first;
			if (!(mod->isActive()))
				continue;
			const Shader* shader = m.second;
			shader->use();

			shader->setBool("useNormalMap", useNormalMap && mod->hasNormalMap());

			shader->setMat4("projection", projection);
			shader->setMat4("view", view);
			shader->setVec3("viewPos", camera->Position);
			glm::mat4 model;
			model *= mod->getModelMatrix();
			glm::mat4 normalModel = glm::transpose(glm::inverse(model));
			shader->setMat4("model", model);
			shader->setMat4("normalModel", normalModel);
			mod->Draw(*shader);
		}

		// GameLogic draw
		DrawFrame();

		// glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
		glfwSwapBuffers(window);
		glfwPollEvents();
	}
}

void RenderEngine::stop()
{
	glfwTerminate();
}

void RenderEngine::setGameLogic(GameLogic& logic)
{
	currentLogic = &logic;
}

GLFWwindow * RenderEngine::getWindow()
{
	return window;
}

float RenderEngine::getDeltaTime()
{
	return deltaTime;
}

void RenderEngine::disableCursor()
{
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
}

void RenderEngine::enableCursor()
{
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
}

void RenderEngine::setBackGround(float R, float G, float B)
{
	RenderEngine::bgR = R;
	RenderEngine::bgG = G;
	RenderEngine::bgB = B;
}

void RenderEngine::setSkyBox(const std::string& _path)
{
	std::string path(_path);
	glGenTextures(1, &sky);
	glBindTexture(GL_TEXTURE_CUBE_MAP, sky);
	std::string sky_faces[6] = { "right.jpg", "left.jpg", "top.jpg", "bottom.jpg", "back.jpg", "front.jpg" };
	if (path.back() != '/')path += '/';
	
	int width, height, nrChannels;
	for (int i = 0; i < 6; i++) {
		unsigned char* data = stbi_load((path + sky_faces[i]).c_str(), &width, &height, &nrChannels, 0);
		if (data) {
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
			stbi_image_free(data);
		}
		else {
			std::cout << "Failed to load cubemap texture at path: " << path + sky_faces[i] << std::endl;
			stbi_image_free(data);
		}
	}
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	static float skyboxVertices[] = {
		// positions          
		-1.0f,  1.0f, -1.0f,
		-1.0f, -1.0f, -1.0f,
		1.0f, -1.0f, -1.0f,
		1.0f, -1.0f, -1.0f,
		1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,

		-1.0f, -1.0f,  1.0f,
		-1.0f, -1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f,  1.0f,
		-1.0f, -1.0f,  1.0f,

		1.0f, -1.0f, -1.0f,
		1.0f, -1.0f,  1.0f,
		1.0f,  1.0f,  1.0f,
		1.0f,  1.0f,  1.0f,
		1.0f,  1.0f, -1.0f,
		1.0f, -1.0f, -1.0f,

		-1.0f, -1.0f,  1.0f,
		-1.0f,  1.0f,  1.0f,
		1.0f,  1.0f,  1.0f,
		1.0f,  1.0f,  1.0f,
		1.0f, -1.0f,  1.0f,
		-1.0f, -1.0f,  1.0f,

		-1.0f,  1.0f, -1.0f,
		1.0f,  1.0f, -1.0f,
		1.0f,  1.0f,  1.0f,
		1.0f,  1.0f,  1.0f,
		-1.0f,  1.0f,  1.0f,
		-1.0f,  1.0f, -1.0f,

		-1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f,  1.0f,
		1.0f, -1.0f, -1.0f,
		1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f,  1.0f,
		1.0f, -1.0f,  1.0f
	};
	glGenVertexArrays(1, &skyVAO);
	glGenBuffers(1, &skyVBO);
	glBindVertexArray(skyVAO);
	glBindBuffer(GL_ARRAY_BUFFER, skyVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GL_FLOAT), (void*)0);
	haveSky = true;
}

void RenderEngine::setTerrain(const std::string& path, const std::string& heightMap, const std::string& texture)
{
	// load the height map
	int width, height, nrChannels;
	unsigned char* data = stbi_load((path + heightMap).c_str(), &width, &height, &nrChannels, 0);
	if (!data)
	{
		std::cout << "Failed to load height map at path: " << path + heightMap << std::endl;
		stbi_image_free(data);
	}
	// calculate the attributes of every vertex
	glm::vec3* g_terrain[MAP_W*MAP_W];
	glm::vec3* g_normal[MAP_W*MAP_W];
	glm::vec2* g_texcoord[MAP_W*MAP_W];
	std::vector<unsigned int> g_index = std::vector<unsigned int>();
	int vindex;
	// calculate except normals
	for (int z = 0; z < MAP_W; z++)
	{
		int n = 0;
		for (int x = 0; x < MAP_W; x++)
		{
			// (z, x)
			vindex = z*MAP_W + x;
			g_terrain[vindex] = new glm::vec3(x*MAP_SCALE - MAP_W * MAP_SCALE / 2, data[vindex * 3] / 8, z*MAP_SCALE - MAP_W * MAP_SCALE / 2);
			g_texcoord[vindex] = new glm::vec2(x, z);
			n++;
			if (z < MAP_W - 1 && n == 2)
			{
				g_index.push_back(vindex - 1);
				g_index.push_back(vindex - 1 + MAP_W);
				g_index.push_back(vindex);
				g_index.push_back(vindex - 1 + MAP_W);
				g_index.push_back(vindex + MAP_W);
				g_index.push_back(vindex);
				n = 1;
			}
		}
	}
	stbi_image_free(data);
	// calculate normals
	for (int i = 0; i < MAP_W*MAP_W; i++)
		g_normal[i] = new glm::vec3(0, 0, 0);
	for (int z = 0; z<MAP_W - 1; z++)
		for (int x = 0; x < MAP_W - 1; x++)
		{
			vindex = z*MAP_W + x;
			glm::vec3 normal1 = glm::normalize(glm::cross(*g_terrain[vindex + MAP_W] - *g_terrain[vindex], *g_terrain[vindex + 1] - *g_terrain[vindex]));
			glm::vec3 normal2 = glm::normalize(glm::cross(*g_terrain[vindex + MAP_W + 1] - *g_terrain[vindex + MAP_W], *g_terrain[vindex + 1] - *g_terrain[vindex + MAP_W]));
			*g_normal[vindex] += normal1;
			*g_normal[vindex + 1] += normal1;
			*g_normal[vindex + 1] += normal2;
			*g_normal[vindex + MAP_W] += normal1;
			*g_normal[vindex + MAP_W] += normal2;
			*g_normal[vindex + MAP_W + 1] += normal2;
		}
	for (int i = 0; i < MAP_W*MAP_W; i++)
		*g_normal[i] = glm::normalize(*g_normal[i]);
	// create Vertex vector
	for (int i = 0; i < MAP_W*MAP_W; i++)
		terrainVertices.push_back(Vertex(*g_terrain[i], *g_normal[i], *g_texcoord[i]));
	// create Texture vector
	unsigned int diffuseMap = Texture::TextureFromFile(texture.c_str(), path);
	unsigned int specularMap = Texture::TextureFromFile(texture.c_str(), path);
	std::vector<Texture> textures = std::vector<Texture>();
	Texture diff; diff.id = diffuseMap; diff.type = Texture::DiffuseType;
	Texture spec; spec.id = specularMap; spec.type = Texture::SpecularType;
	textures.push_back(diff);
	textures.push_back(spec);
	// add the terrain Model
	Mesh terrain(terrainVertices, g_index, textures, 32.0f);
	std::vector<Mesh> meshes;
	meshes.push_back(terrain);
	Model* terrainModel = new Model(meshes);
	addModel(*terrainModel);
}

float RenderEngine::getHeight(float x, float z)
{
	float leftBorder = -MAP_W*MAP_SCALE / 2;
	float rightBorder = MAP_W*MAP_SCALE / 2 - MAP_SCALE;
	if (x<=leftBorder || x>=rightBorder || z<=leftBorder || z>=rightBorder)
		return 0.0f;
	// convert to coordinate in the height map
	float mapX = (x + MAP_W*MAP_SCALE / 2) / MAP_SCALE;
	float mapZ = (z + MAP_W*MAP_SCALE / 2) / MAP_SCALE;
	int col0 = (int)mapX;
	int col1 = col0 + 1;
	int row0 = (int)mapZ;
	int row1 = row0 + 1;
	float h00 = terrainVertices[row0*MAP_W + col0].Position.y;
	float h01 = terrainVertices[row0*MAP_W + col1].Position.y;
	float h10 = terrainVertices[row1*MAP_W + col0].Position.y;
	float h11 = terrainVertices[row1*MAP_W + col1].Position.y;
	float tx = mapX - col0;
	float tz = mapZ - row0;
	// interpolation
	if (tx + tz < 1)
		return (1 - tx - tz)*h00 + tx*h01 + tz*h10;
	else
		return (tx + tz - 1)*h11 + (1 - tx)*h10 + (1 - tz)*h01;
}

glm::vec3 RenderEngine::getNormal(float x, float z)
{
	float leftBorder = -MAP_W*MAP_SCALE / 2;
	float rightBorder = MAP_W*MAP_SCALE / 2 - MAP_SCALE;
	if (x <= leftBorder || x >= rightBorder || z <= leftBorder || z >= rightBorder)
		return glm::vec3(0.0f,1.0f,0.0f);
	// convert to coordinate in the height map
	float mapX = (x + MAP_W*MAP_SCALE / 2) / MAP_SCALE;
	float mapZ = (z + MAP_W*MAP_SCALE / 2) / MAP_SCALE;
	int col0 = (int)mapX;
	int col1 = col0 + 1;
	int row0 = (int)mapZ;
	int row1 = row0 + 1;
	float tx = mapX - col0;
	float tz = mapZ - row0;
	glm::vec3 v0 = terrainVertices[row0*MAP_W + col0].Position;
	glm::vec3 v1 = terrainVertices[row1*MAP_W + col0].Position;
	glm::vec3 v2 = terrainVertices[row0*MAP_W + col1].Position;
	glm::vec3 v3 = terrainVertices[row1*MAP_W + col1].Position;

	if (tx + tz < 1)
		return glm::normalize(glm::cross(v1 - v0, v2 - v0));
	else
		return glm::normalize(glm::cross(v3 - v1, v2 - v1));
}

void RenderEngine::setCamera(Camera & camera)
{
	RenderEngine::camera = &camera;
}

void RenderEngine::setNormalMapRender(bool set)
{
	useNormalMap = set;
}

void RenderEngine::addModel(Model & model, const Shader& shader)
{
	models.push_back(std::pair<Model*, const Shader*>(&model, &shader));
}

bool RenderEngine::deleteModel(Model & model)
{
	for (std::vector<ModelAndShader>::iterator i = models.begin(); i != models.end(); i++) {
		if ((*i).first == &model) {
			models.erase(i);
			return true;
		}
	}
	return false;
}

void RenderEngine::FramebufferSizeCallback(GLFWwindow * window, int width, int height)
{
	if (currentLogic) {
		currentLogic->FramebufferSizeCallback(window, width, height);
	}
	else {
		_default_framebuffer_size_callback(window, width, height);
	}
}

void RenderEngine::ProcessInput(GLFWwindow * window, float deltaTime)
{
	if (currentLogic) {
		currentLogic->ProcessInput(window, deltaTime);
	}
}

void RenderEngine::MouseCallback(GLFWwindow * window, double xPos, double yPos)
{
	if (currentLogic) {
		currentLogic->MouseCallback(window, xPos, yPos);
	}
}

void RenderEngine::ScrollCallback(GLFWwindow * window, double xoffset, double yoffset)
{
	if (currentLogic) {
		currentLogic->ScrollCallback(window, xoffset, yoffset);
	}
}

void RenderEngine::DrawFrame(void)
{
	if (currentLogic) {
		currentLogic->DrawFrame();
	}
}

void _default_framebuffer_size_callback(GLFWwindow * window, int width, int height)
{
	glViewport(0, 0, width, height);
}

void RenderEngine::setDirLight(int index, glm::vec3 direction, glm::vec3 ambient, glm::vec3 diffuse, glm::vec3 specular) {
	defaultShader.setDirLight(index, direction, ambient, diffuse, specular);
}
void RenderEngine::addDirLight(glm::vec3 direction, glm::vec3 ambient, glm::vec3 diffuse, glm::vec3 specular) {
	defaultShader.addDirLight(direction, ambient, diffuse, specular);
}
void RenderEngine::setPointLight(int index, glm::vec3 position, glm::vec3 ambient, glm::vec3 diffuse, glm::vec3 specular,
	float constant, float linear, float quadratic) {
	defaultShader.setPointLight(index, position, ambient, diffuse, specular, constant, linear, quadratic);
}
void RenderEngine::addPointLight(glm::vec3 position, glm::vec3 ambient, glm::vec3 diffuse, glm::vec3 specular,
	float constant, float linear, float quadratic) {
	defaultShader.addPointLight(position, ambient, diffuse, specular, constant, linear, quadratic);
}
void RenderEngine::setSpotLight(int index, glm::vec3 position, glm::vec3 direction, glm::vec3 diffuse, glm::vec3 specular,
	float constant, float linear, float quadratic, float cutoff, float outerCutoff) {
	defaultShader.setSpotLight(index, position, direction, diffuse, specular, constant, linear, quadratic, cutoff, outerCutoff);
}
void RenderEngine::addSpotLight(glm::vec3 position, glm::vec3 direction, glm::vec3 diffuse, glm::vec3 specular,
	float constant, float linear, float quadratic, float cutoff, float outerCutoff) {
	defaultShader.addSpotLight(position, direction, diffuse, specular, constant, linear, quadratic, cutoff, outerCutoff);
}
