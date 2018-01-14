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
LightShader RenderEngine::defaultShader(0, 0, 0);

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

		glm::mat4 projection = glm::perspective(glm::radians(camera->Zoom), RenderEngine::SCR_WIDTH / RenderEngine::SCR_HEIGHT, 0.1f, 100.0f);
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
