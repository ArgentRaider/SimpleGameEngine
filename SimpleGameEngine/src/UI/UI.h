#ifndef SIMPLE_UI_H
#define SIMPLE_UI_H

#include <string>
#include <map>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <ShaderProgram/Shader.h>
#include <ft2build.h>
#include FT_FREETYPE_H

/// Holds all state information relevant to a character as loaded using FreeType
struct Character {
	GLuint     TextureID;  // ����������ID
	glm::ivec2 Size;       // ���δ�С
	glm::ivec2 Bearing;    // �ӻ�׼�ߵ�������/������ƫ��ֵ
	GLuint     Advance;    // ԭ�����һ������ԭ��ľ���
};
class GameLogic;
class MainGameLogic;

class UI
{
public:
	static void init();

	static void renderText(std::string text, GLfloat x, GLfloat y, GLfloat scale, glm::vec3 color = glm::vec3(1.0f, 1.0f, 1.0f));
	static void drawRec(GLfloat center_x, GLfloat center_y, GLfloat width, GLfloat height, glm::vec4 RGBA = glm::vec4(0.0f, 0.0f, 0.0f, 0.5f));
	static void drawRecP(GLfloat center_x_p, GLfloat center_y_p, GLfloat width_p, GLfloat height_p, glm::vec4 RGBA = glm::vec4(0.0f, 0.0f, 0.0f, 0.5f));
	static void drawMenu();

	// entry and cursor
	static void addEntry(int id = entryID);
	static void subEntry(int id = entryID);
	static void addCursor(void);
	static void subCursor(void);
	static void pressEnter(MainGameLogic* mainGame);
	static void pressEsc(MainGameLogic* mainGame);

	// blood : [0, 100]
	static void drawBlood();
	static float getBlood();
	static void setBlood(float newBlood);
	static void addBlood(float delta);
	static void subBlood(float delta);
	static void screenshot();

private:
	static std::map<GLchar, Character> Characters;
	static GLuint UI_VAO, UI_VBO;
	static glm::vec4 highlight;
	static glm::vec4 lowlight;
	static glm::vec4 redlight;
	static void RenderText(Shader &shader, std::string text, GLfloat x, GLfloat y, GLfloat scale, glm::vec3 color);
	static void drawMainMenu();
	static void drawEditMenu();
	static void drawMenuTool(int myEntryID, int cursorPosMax, char** titles);
	static void drawEditTank();
	static void drawEditLight();
	static void drawimportMenu();
	static void drawVideoMenu();
	static int getCursor(); 
	static void setNewLight();

	// entry and cursor
	static int entryPos[50];
	static int entryPosMax[50];
	static int entryID;
	static int cursorPos[50];
	static int cursorPosMax[50];

	// const
	static char *titles_main[50];
	static char *titles_edit[50];

	// blood
	static float blood;

	// screen shot flag
	static bool shotflag;
};

#endif