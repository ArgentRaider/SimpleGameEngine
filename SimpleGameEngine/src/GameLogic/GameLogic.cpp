#include "GameLogic.h"

#include <glm\glm.hpp>
#include <glm\gtc\matrix_transform.hpp>

#include <RenderEngine/RenderEngine.h>

MainGameLogic::MainGameLogic()
{
	// set a shader using Phong illumination model
	ourShader = new LightShader(0, 0, 0);
	ourShader->init();

	// get a model wearing nanosuit!
	tank1 = new Tank("res/models/shen/shen.obj");
	barrel1 = new Model("res/models/barrel/barrel.obj");
	tank2 = new Tank("res/models/shen2/shen2.obj");
	barrel2 = new Model("res/models/barrel2/barrel2.obj");
	tank1->addBarrel(barrel1);
	tank2->addBarrel(barrel2);
	ourModel = tank1;
	//ourModel = new Model("res/models/A-32/a-32_hull.obj");
	// adjust our model
	double deltay1 = -tank1->getCollider().ymin;
	tank1->Translate(glm::vec3(0.0f, deltay1, 0.0f));
	tank1->Scale(glm::vec3(0.2f, 0.2f, 0.2f));

	double deltay2 = -tank2->getCollider().ymin;
	tank2->Translate(glm::vec3(50.0f, deltay2, 50.0f));
	tank2->Scale(glm::vec3(0.2f, 0.2f, 0.2f));

	// add it to display
	RenderEngine::addModel(*tank1);
	RenderEngine::addModel(*barrel1);
	RenderEngine::addModel(*tank2);
	RenderEngine::addModel(*barrel2);

	// add a third-person camera following our nanosuit model!
	camera = new ThirdPersonCamera(*ourModel);
	//camera = new Camera();
	RenderEngine::setCamera(camera->camera);

	// set a soft background color
	RenderEngine::setBackGround(0.2f, 0.3f, 0.3f);
	RenderEngine::setSkyBox("res/textures/skybox");
	RenderEngine::setTerrain("res/textures/", "terrain.bmp", "land.bmp");

	// Set light in Fragment Shader
	glm::vec3 lightDir(0.8f, 0.6f, 1.0f);
	glm::vec3 defaultAmbient(1.f, 1.0f, 1.0f);
	glm::vec3 defaultDiffuse(0.5f, 0.5f, 0.5f);
	glm::vec3 defaultSpecular(1.0f, 1.0f, 1.0f);
	glm::vec3 pointLightPositions[] = {
		glm::vec3(50.0f, 5.0f, 50.0f),
		glm::vec3(50.0f, 5.0f, -50.0f),
		glm::vec3(-50.0f, 5.0f, 50.0f),
		glm::vec3(-50.0f, 5.0f, -50.0f)
	};
	float constant = 1.0f, linear = 0.09f, quadratic = 0.032f;
	
	RenderEngine::addDirLight(lightDir, defaultAmbient, defaultDiffuse, defaultSpecular);
	for (glm::vec3 position : pointLightPositions) {
		RenderEngine::addPointLight(position, defaultAmbient, defaultDiffuse, defaultSpecular, constant, linear, quadratic);
	}
	
	// Draw a rectangle using self-defined data
	/*
	Vertex topRight(glm::vec3(50.0f, 0.0f, 50.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec2(1.0f, 1.0f)),
		bottomRight(glm::vec3(50.0f, 0.0f, -50.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec2(1.0f, 0.0f)),
		topLeft(glm::vec3(-50.0f, 0.0f, 50.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec2(0.0f, 1.0f)),
		bottomLeft(glm::vec3(-50.0f, 0.0f, -50.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec2(0.0f, 0.0f));
	std::vector<Vertex> vertices = { topRight, bottomRight, topLeft, bottomLeft };
	std::vector<unsigned int> indices = {
		0, 1, 2,
		1, 2, 3,
	};
	diffuseMap = Texture::TextureFromFile("container2.png", "res/textures");
	specularMap = Texture::TextureFromFile("container2_specular.png", "res/textures");

	std::vector<Texture> textures = std::vector<Texture>();
	Texture diff;	diff.id = diffuseMap;	diff.type = Texture::DiffuseType;
	Texture spec;	spec.id = specularMap;	spec.type = Texture::SpecularType;
	textures.push_back(diff);	textures.push_back(spec);
	rectangle = new Mesh(vertices, indices, textures, 32.0f);
	std::vector<Mesh> meshes;
	meshes.push_back(*rectangle);
	recModel = new Model(meshes);
	RenderEngine::addModel(*recModel);
	*/
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
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
		camera->ProcessKeyboard(Camera::FORWARD, deltaTime);
		glm::vec3 shift;
		shift = glm::normalize(glm::vec3(-ourModel->Front.x, 0.0f, ourModel->Front.z)) * 6.0f * deltaTime;
		ourModel->Translate(shift);
		ourModel->shift += shift;
	}
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
		camera->ProcessKeyboard(Camera::BACKWARD, deltaTime);
		glm::vec3 shift;
		shift = -glm::normalize(glm::vec3(-ourModel->Front.x, 0.0f, ourModel->Front.z)) * 6.0f * deltaTime;
		ourModel->Translate(shift);
		ourModel->shift += shift;
	}
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
		ourModel->Rotate(rotateDelt, glm::vec3(0, 1.0f, 0));
		ourModel->backout();
	}
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
		ourModel->Rotate(-rotateDelt, glm::vec3(0, 1.0f, 0));
		ourModel->backout();
	}
	if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
		ourModel->adjustBarrelUp();
	if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
		ourModel->adjustBarrelDown();

	if (glfwGetKey(window, GLFW_KEY_L) == GLFW_PRESS)
		changeTank();

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

void MainGameLogic::changeTank() {
	if (ourModel == tank1) {
		Sleep(100);
		ourModel = tank2;
		printf("Tank1\n");
		camera->changeTank(ourModel);
	}
	else if(ourModel == tank2)
	{
		Sleep(100);
		ourModel = tank1;
		printf("Tank2\n");
		camera->changeTank(ourModel);
	}

}


