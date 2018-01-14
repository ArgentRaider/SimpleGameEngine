#include "GameLogic.h"

#include <glm\glm.hpp>
#include <glm\gtc\matrix_transform.hpp>

#include <RenderEngine/RenderEngine.h>

MainGameLogic::MainGameLogic()
{
	// set a shader using Phong illumination model
	ourShader = new LightShader(0, 0, 0);
	ourShader->init();
	RenderEngine::setNormalMapRender(true);

	// set a soft background color
	RenderEngine::setBackGround(0.2f, 0.3f, 0.3f);
//	RenderEngine::setSkyBox("res/textures/skybox");
	

	// Set light in Fragment Shader
	glm::vec3 lightDir(0.8f, 0.6f, -1.0f);
	glm::vec3 defaultAmbient(0.2f, 0.2f, 0.2f);
	glm::vec3 defaultDiffuse(0.6f, 0.6f, 0.6f);
	glm::vec3 defaultSpecular(1.0f, 1.0f, 1.0f);
	glm::vec3 pointLightPositions[] = {
		glm::vec3(50.0f, 5.0f, 50.0f),
		glm::vec3(50.0f, 5.0f, -50.0f),
		glm::vec3(-50.0f, 5.0f, 50.0f),
		glm::vec3(-50.0f, 5.0f, -50.0f)
	};
	float constant = 1.0f, linear = 0.09f, quadratic = 0.032f;
	RenderEngine::addDirLight(lightDir, defaultAmbient, defaultDiffuse, defaultSpecular);
	//for (glm::vec3 position : pointLightPositions) {
	//	RenderEngine::addPointLight(position, defaultAmbient, defaultDiffuse, defaultSpecular, constant, linear, quadratic);
	//}

	// get a cyborg model!
	ourModel = new Model("res/models/cyborg/cyborg.obj");
	//ourModel = new Model("res/models/test/fuck.obj");
	// adjust our model
	double deltay = -ourModel->getCollider().ymin;
	ourModel->Translate(glm::vec3(0.0f, deltay, 0.0f));
	ourModel->Scale(glm::vec3(0.5f, 0.5f, 0.5f));
	// add it to display
	RenderEngine::addModel(*ourModel);

	// add a third-person camera following our nanosuit model!
	camera = new ThirdPersonCamera(*ourModel);
	//camera = new Camera();
	RenderEngine::setCamera(camera->camera);

	// Draw a rectangle using self-defined data
	Vertex topRight(glm::vec3(50.0f, 0.0f, 50.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec2(1.0f, 1.0f)),
		bottomRight(glm::vec3(50.0f, 0.0f, -50.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec2(1.0f, 0.0f)),
		topLeft(glm::vec3(-50.0f, 0.0f, 50.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec2(0.0f, 1.0f)),
		bottomLeft(glm::vec3(-50.0f, 0.0f, -50.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec2(0.0f, 0.0f));
	topRight.Tangent = bottomLeft.Tangent = bottomRight.Tangent = topLeft.Tangent 
		= glm::vec3(1.0f, 0.0f, 0.0f);
	topRight.Bitangent = bottomLeft.Bitangent = bottomRight.Bitangent = topLeft.Bitangent 
		= glm::vec3(0.0f, 0.0f, 1.0f);
	std::vector<Vertex> vertices = { topRight, bottomRight, topLeft, bottomLeft };
	std::vector<unsigned int> indices = {
		0, 1, 2,
		1, 2, 3,
	};
	diffuseMap = Texture::TextureFromFile("brickwall.jpg", "res/textures");
	specularMap = Texture::TextureFromFile("brickwall.jpg", "res/textures");
	normalMap = Texture::TextureFromFile("brickwall_normal.jpg", "res/textures");

	std::vector<Texture> textures = std::vector<Texture>();
	Texture diff;	diff.id = diffuseMap;	diff.type = Texture::DiffuseType;
	Texture spec;	spec.id = specularMap;	spec.type = Texture::SpecularType;
	Texture norm;	norm.id = normalMap;	norm.type = Texture::NormalType;
	textures.push_back(diff);	textures.push_back(spec);	textures.push_back(norm);
	rectangle = new Mesh(vertices, indices, textures, 64.0f);
	std::vector<Mesh> meshes;
	meshes.push_back(*rectangle);
	recModel = new Model(meshes, true);
	RenderEngine::addModel(*recModel);
}

MainGameLogic::~MainGameLogic()
{
	free(recModel);
	free(rectangle);
	free(ourModel);
	free(camera);
	free(ourShader);
}

void MainGameLogic::DrawFrame(void)
{
}

void MainGameLogic::ProcessInput(GLFWwindow* window, float deltaTime)
{
	static bool pressB = false;

	// Exit
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);

	// Move the camera
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		camera->ProcessKeyboard(Camera::FORWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		camera->ProcessKeyboard(Camera::BACKWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		camera->ProcessKeyboard(Camera::LEFT, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		camera->ProcessKeyboard(Camera::RIGHT, deltaTime);

	if (glfwGetKey(window, GLFW_KEY_B) == GLFW_PRESS) {
		pressB = true;
	}
	else if (pressB && glfwGetKey(window, GLFW_KEY_B) == GLFW_RELEASE) {
		pressB = false;
		disableCursor = !disableCursor;
		if (disableCursor) {
			firstMouse = true;
			RenderEngine::disableCursor();		// Disable the cursor for an FPS camera
		}
		else {
			RenderEngine::enableCursor();
		}
	}
}

void MainGameLogic::MouseCallback(GLFWwindow* window, double xPos, double yPos)
{
	static float lastX = 0, lastY = 0;

	if (disableCursor) {
		if (firstMouse) {
			lastX = (float)xPos;
			lastY = (float)yPos;
			firstMouse = false;
		}
		float xoffset = (float)xPos - lastX, yoffset = lastY - (float)yPos;
		lastX = (float)xPos;
		lastY = (float)yPos;

		camera->ProcessMouseMovement(xoffset, yoffset);
	}
}

void MainGameLogic::ScrollCallback(GLFWwindow * window, double xoffset, double yoffset)
{
	camera->ProcessMouseScroll(yoffset);
}


