#ifndef SIMPLE_GAMELOGIC_H
#define SIMPLE_GAMELOGIC_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <ShaderProgram/Shader.h>
#include <Model/Model.h>
#include <Model/Mesh.h>
#include <Model/Tank/Tank.h>
#include <Camera/ThirdPersonCamera.h>
#include <UI/UI.h>

// Abstract class GameLogic
// You may create your own game logic by overriding the virtual functions and add some COOL things!
// If you want to have MORE THAN ONE logics in your game, it is suggested to create main game logic, and let this main logic
//  to control different logics.
class GameLogic
{
public:

	void FramebufferSizeCallback(GLFWwindow* window, int width, int height) {
		glViewport(0, 0, width, height);
	}

	virtual void ProcessInput(GLFWwindow* window, float deltaTime) = 0;
	virtual void MouseCallback(GLFWwindow* window, double xPos, double yPos) = 0;
	virtual void ScrollCallback(GLFWwindow* window, double xoffset, double yoffset) = 0;
	virtual void DrawFrame(void) = 0;
};


// This is an example logic so I decide to put it here.
class MainGameLogic :public GameLogic 
{
public:
	MainGameLogic();	// Set all things you need,
	~MainGameLogic();	// and free them when the game ends

	void ProcessInput(GLFWwindow* window, float deltaTime);
	void MouseCallback(GLFWwindow* window, double xPos, double yPos);
	void ScrollCallback(GLFWwindow* window, double xoffset, double yoffset);
	void DrawFrame(void);
	void closeMenu();
private:
	// Shader(s)
	LightShader *ourShader;

	// Camera(s)
	ThirdPersonCamera *currentCamera;
	ThirdPersonCamera *camera1;
	ThirdPersonCamera *camera2;
	//Camera *camera;
	
	// Model(s)
	Tank *ourModel;
	Tank *tank1;
	Model *barrel1;
	Tank *tank2;
	Model *barrel2;
	unsigned int diffuseMap;
	unsigned int specularMap;
	unsigned int normalMap;
	Mesh* rectangle;
	Model* recModel;
	float rotateDelt = 2.0;

	// some settings
	bool disableCursor = false;
	bool firstMouse = true;
	bool disableMenu = false;

	void changeTank();
};



#endif 
