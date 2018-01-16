#include "GameLogic.h"

#include <glm\glm.hpp>
#include <glm\gtc\matrix_transform.hpp>

#include <RenderEngine/RenderEngine.h>

bool MainGameLogic::attackFinished = false;
int MainGameLogic::turnState = 0;

MainGameLogic::MainGameLogic()
{
	// set a shader using Phong illumination model
	ourShader = new LightShader(0, 0, 0);
	ourShader->init();

	RenderEngine::setNormalMapRender(true);

	// get a model wearing nanosuit!
	tank1 = new Tank("res/models/shen/shen.obj");
	barrel1 = new Model("res/models/barrel/barrel.obj");
	tank1->setShift(0, 30, 0);
	barrel1->setShift(0, 30, 0);
	tank2 = new Tank("res/models/shen2/shen2.obj");
	barrel2 = new Model("res/models/barrel2/barrel2.obj");
	tank2->setShift(10, 29.625, 10);
	barrel2->setShift(10, 29.625, 10);
	tank1->addBarrel(barrel1);
	tank2->addBarrel(barrel2);
	ourModel = tank1;

	// adjust our model
	double deltay1 = -tank1->getCollider().ymin;
	tank1->Translate(glm::vec3(0.0f, deltay1 + 30 / 0.2, 0.0f));
	tank1->Scale(glm::vec3(0.2f, 0.2f, 0.2f));

	double deltay2 = -tank2->getCollider().ymin;
	tank2->Translate(glm::vec3(50.0f, deltay2 + 29.625 / 0.2, 50.0f));
	tank2->Scale(glm::vec3(0.2f, 0.2f, 0.2f));

	tank1->Rotate(180, glm::vec3(0.0, 1.0, 0.0), ANGLE, UNADAPT);
	tank2->Rotate(180, glm::vec3(0.0, 1.0, 0.0), ANGLE, UNADAPT);

	// add it to display
	RenderEngine::addModel(*tank1);
	RenderEngine::addModel(*barrel1, false);
	RenderEngine::addModel(*tank2);
	RenderEngine::addModel(*barrel2, false);

	// add a third-person camera following our nanosuit model!
	camera1 = new ThirdPersonCamera(*tank1);
	camera2 = new ThirdPersonCamera(*tank2);

	currentCamera = camera1;
	//camera = new Camera();
	RenderEngine::setCamera(currentCamera->camera);

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

	// UI init
	UI::init();

	//for (glm::vec3 position : pointLightPositions) {
	//	RenderEngine::addPointLight(position, defaultAmbient, defaultDiffuse, defaultSpecular, constant, linear, quadratic);
	//}

	// Draw a rectangle using self-defined data
	/*
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
	*/
}

MainGameLogic::~MainGameLogic()
{
	free(recModel);
	free(rectangle);
	free(ourModel);
	free(camera1);
	free(camera2);
	free(ourShader);
}

void MainGameLogic::DrawFrame(void)
{

	if (disableMenu) {
		UI::drawBlood(turnState);
		UI::drawPower();
		UI::drawCountDown(this);

		if (turnState == 0 || turnState == 2) {
			remainingTime = 15.0f - (glfwGetTime() - timeStamp);
			if (remainingTime <= 0.0f) {
				changeTank();
				timeStamp = glfwGetTime();
				remainingTime = 15.0f;
				if (ourModel == tank1) turnState = 0;
				else if (ourModel == tank2) turnState = 2;
			}
			else if (chargeFinished) {
				chargeFinished = false;
				turnState += 1;
			}
		}
		else if (turnState == 1 || turnState == 3) {
			if (attackFinished) {
				attackFinished = false;
				timeStamp = glfwGetTime();
				waitForExplosion = true;
			}
			if (waitForExplosion) {
				if (glfwGetTime() - timeStamp > 1.0f) {
					waitForExplosion = false;
					changeTank();
					timeStamp = glfwGetTime();
					remainingTime = 15.0f;
					if (ourModel == tank1) turnState = 0;
					else if (ourModel == tank2) turnState = 2;
				}
			}
		}
	}

	UI::screenshot();
	if (!disableMenu)
		UI::drawMenu();
}

void MainGameLogic::ProcessInput(GLFWwindow* window, float deltaTime)
{
	static bool pressB = false, pressESC = false, pressW = false,
		pressS = false, pressA = false, pressD = false, pressEnter = false,
		pressSpace = false;

	// process ESC, switch between game and menu
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
		pressESC = true;
	}
	else if (pressESC && glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_RELEASE) {
		pressESC = false;
		// press ESC in menu UI
		if (!disableMenu)
			UI::pressEsc(this);
		// show menu UI
		else
			disableMenu = !disableMenu;
	}

	// process keys in the menu
	if (!disableMenu) {
		// Move the cursor
		if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
			pressW = true;
		else if (pressW && glfwGetKey(window, GLFW_KEY_W) == GLFW_RELEASE) {
			pressW = false;
			UI::subEntry();
		}
		if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
			pressS = true;
		else if (pressS && glfwGetKey(window, GLFW_KEY_S) == GLFW_RELEASE) {
			pressS = false;
			UI::addEntry();
		}
		if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
			pressA = true;
		else if (pressA && glfwGetKey(window, GLFW_KEY_A) == GLFW_RELEASE) {
			pressA = false;
			UI::subCursor();
		}
		if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
			pressD = true;
		else if (pressD && glfwGetKey(window, GLFW_KEY_D) == GLFW_RELEASE) {
			pressD = false;
			UI::addCursor();
		}
		if (glfwGetKey(window, GLFW_KEY_ENTER) == GLFW_PRESS)
			pressEnter = true;
		else if (pressEnter && glfwGetKey(window, GLFW_KEY_ENTER) == GLFW_RELEASE) {
			pressEnter = false;
			UI::pressEnter(this);
		}
	}

	// process keys in the game
	else {
		// prepare power of tank
		if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) {
			UI::charge(deltaTime);
			timeStamp += deltaTime;
			pressSpace = true;
		}
		if (pressSpace && glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_RELEASE) {
			pressSpace = false;
			float power = UI::finishCharge();
			chargeFinished = true;
			//printf("%f,%f,%f\n", ourModel->barrel->Front.x, ourModel->barrel->Front.y, ourModel->barrel->Front.z);
			glm::vec4 temp = glm::vec4(-ourModel->barrel->Front.x, ourModel->barrel->Front.y, ourModel->barrel->Front.z, ourModel->barrel->Front.w);
			RenderEngine::cball.set(BasicComp::drawSphere(0.1f), temp*power*0.5f, ourModel->shift);
		}
		// Move the camera
		if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {

			float x0 = ourModel->shift.x;
			float y0 = ourModel->shift.y;
			float z0 = ourModel->shift.z;
			float x1 = ourModel->shift.x - ourModel->Front.x * 6.0f * deltaTime;
			float z1 = ourModel->shift.z + ourModel->Front.z * 6.0f * deltaTime;
			float y1 = RenderEngine::getHeight(x1, z1);
			ourModel->Translate(glm::vec3(x1 - x0, y1 - y0, z1 - z0));
			if (RenderEngine::existCollision(ourModel)) {
				ourModel->Translate(glm::vec3(x0 - x1, y0 - y1, z0 - z1));
			}
			else {
				ourModel->shift = glm::vec3(x1, y1, z1);
				currentCamera->ProcessKeyboard(glm::vec3(x1 - x0, y1 - y0, z1 - z0));
			}
			AdaptTerrain(ourModel->shift, ourModel);
		}
		if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {

			float x0 = ourModel->shift.x;
			float y0 = ourModel->shift.y;
			float z0 = ourModel->shift.z;
			float x1 = ourModel->shift.x + ourModel->Front.x * 6.0f * deltaTime;
			float z1 = ourModel->shift.z - ourModel->Front.z * 6.0f * deltaTime;
			float y1 = RenderEngine::getHeight(x1, z1);
			ourModel->Translate(glm::vec3(x1 - x0, y1 - y0, z1 - z0));
			if (RenderEngine::existCollision(ourModel)) {
				ourModel->Translate(glm::vec3(x0 - x1, y0 - y1, z0 - z1));
			}
			else {
				ourModel->shift = glm::vec3(x1, y1, z1);
				currentCamera->ProcessKeyboard(glm::vec3(x1 - x0, y1 - y0, z1 - z0));
			}
			AdaptTerrain(ourModel->shift, ourModel);
		}
		if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
			ourModel->Rotate(rotateDelt, glm::vec3(0, 1.0f, 0), ANGLE, UNADAPT);
			if (RenderEngine::existCollision(ourModel)) {
				ourModel->Rotate(-rotateDelt, glm::vec3(0, 1.0f, 0), ANGLE, UNADAPT);
			}
			ourModel->backout();
		}
		if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
			ourModel->Rotate(-rotateDelt, glm::vec3(0, 1.0f, 0), ANGLE, UNADAPT);
			if (RenderEngine::existCollision(ourModel)) {
				ourModel->Rotate(rotateDelt, glm::vec3(0, 1.0f, 0), ANGLE, UNADAPT);
			}
			ourModel->backout();
		}
		if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
			ourModel->adjustBarrelUp();
		if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
			ourModel->adjustBarrelDown();

		/*if (glfwGetKey(window, GLFW_KEY_L) == GLFW_PRESS)
		changeTank();*/

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

		currentCamera->ProcessMouseMovement(xoffset, yoffset);
		if (currentCamera->Position.y < RenderEngine::getHeight(currentCamera->Position.x, currentCamera->Position.z)) {
			currentCamera->ProcessMouseMovement(-xoffset, -yoffset);
		}
	}
}

void MainGameLogic::ScrollCallback(GLFWwindow * window, double xoffset, double yoffset)
{
	currentCamera->ProcessMouseScroll(yoffset);
	if (currentCamera->Position.y < RenderEngine::getHeight(currentCamera->Position.x, currentCamera->Position.z)) {
		currentCamera->ProcessMouseScroll(-yoffset);
	}
}

void MainGameLogic::changeTank() {
	if (ourModel == tank1) {
		Sleep(100);
		ourModel = tank2;
		currentCamera = camera2;
		RenderEngine::setCamera(currentCamera->camera);
	}
	else if (ourModel == tank2)
	{
		Sleep(100);
		ourModel = tank1;
		currentCamera = camera1;
		RenderEngine::setCamera(currentCamera->camera);
	}

}

void MainGameLogic::closeMenu()
{
	disableMenu = !disableMenu;
}

void MainGameLogic::startGame()
{
	timeStamp = glfwGetTime();
	turnState = 0;
	remainingTime = 15.0f;
}

void MainGameLogic::AdaptTerrain(glm::vec3 pos, Model *tank) {


	glm::vec3 normalTerrain = RenderEngine::getNormal(pos.x, pos.z);
	glm::vec3 tn = glm::normalize(glm::vec3(0.0, normalTerrain.y, normalTerrain.z));
	glm::vec3 v;

	glm::mat4 temp0;

	v = glm::normalize(glm::vec3(tank->normal.x, tank->normal.y, 0.0));
	float cosRadian = abs(v.x / v.y);
	float dir = v.x;

	if (d < 0) {
		tank->Rotate(-r, glm::vec3(0.0, 0.0, 1.0), RADIAN, ADAPT);
		temp0 = glm::rotate(temp0, -r, glm::vec3(0.0, 0.0, 1.0));
	}
	else if (d > 0) {
		tank->Rotate(r, glm::vec3(0.0, 0.0, 1.0), RADIAN, ADAPT);
		temp0 = glm::rotate(temp0, r, glm::vec3(0.0, 0.0, 1.0));
	}

	glm::mat4 temp1;
	v = glm::normalize(glm::vec3(0.0, normalTerrain.y, normalTerrain.z));
	cosRadian = abs(v.z / v.y);
	dir = tank->normal.z - v.z;

	if (dir < -0.1) {
		tank->Rotate(-glm::atan(cosRadian), glm::vec3(1.0, 0.0, 0.0), RADIAN, ADAPT);
		temp1 = glm::rotate(temp1, -glm::atan(cosRadian), glm::vec3(1.0, 0.0, 0.0));
	}
	else if (dir > 0.1) {
		tank->Rotate(glm::atan(cosRadian), glm::vec3(1.0, 0.0, 0.0), RADIAN, ADAPT);
		temp1 = glm::rotate(temp1, glm::atan(cosRadian), glm::vec3(1.0, 0.0, 0.0));
	}
	if (dir < -0.1 || dir > 0.1) {
		glm::vec4 vTemp;
		vTemp = (glm::vec4(tank->normal, 1.0) * temp1);
		vTemp = vTemp / vTemp.w;
		tank->normal = glm::normalize(glm::vec3(vTemp.x, vTemp.y, vTemp.z));
	}


	glm::mat4 temp2;
	v = glm::normalize(glm::vec3(normalTerrain.x, normalTerrain.y, 0.0));
	cosRadian = abs(v.x / v.y);
	dir = tank->normal.x - v.x;


	if (dir < 0) {
		tank->Rotate(glm::atan(cosRadian), glm::vec3(0.0, 0.0, 1.0), RADIAN, ADAPT);
		temp2 = glm::rotate(temp2, glm::atan(cosRadian), glm::vec3(0.0, 0.0, 1.0));
	}
	else if (dir > 0) {
		tank->Rotate(-glm::atan(cosRadian), glm::vec3(0.0, 0.0, 1.0), RADIAN, ADAPT);
		temp2 = glm::rotate(temp2, -glm::atan(cosRadian), glm::vec3(0.0, 0.0, 1.0));
	}

	d = dir;
	r = glm::atan(cosRadian);

}