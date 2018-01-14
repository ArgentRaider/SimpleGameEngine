#include "UI.h"
#include <RenderEngine\RenderEngine.h>
#include <GameLogic/GameLogic.h>
#include <iostream>  
#include <string>

std::map<GLchar, Character> UI::Characters;
GLuint UI::UI_VAO, UI::UI_VBO;

int UI::entryPos[50];
int UI::entryPosMax[50] = {5,2,3,20,2,2};
int UI::entryID = 0;
int UI::cursorPos[50] = { 0,0,0,0,0,0,0,0,0,0,50,5, 50, 8, 6, 10, 10, 10, 10, 5, 5, 5, 10, 10, 10, 10, 9, 32 };
int UI::cursorPosMax[50] = { 9,9,9,3,100,100,9,9,0,0,100, 100, 100, 11,11,11,11,11,11,11,11,11,11,11,11,100,100,1000 };
glm::vec4 UI::highlight(glm::vec4(0.5f, 0.5f, 0.5f, 0.5f));
glm::vec4 UI::lowlight(glm::vec4(0.0f, 0.0f, 0.0f, 0.5f));
glm::vec4 UI::redlight(glm::vec4(1.0f, 0.5f, 0.5f, 0.5f));
char *UI::titles_main[50] = { "resume game", "edit menu", "import and export", "video and screenshot", "exit game" };
char *UI::titles_edit[50] = { "edit my tank" , "edit light" };

/*
All menu content:
	Main menu						(entryID=0
	0.	Resume game
	1.	Edit menu					(entryID=1
		a)	edit my tank			(entryID=2
			i.	edit material		(cursorID=0
			ii.	edit texture		(cursorID=1
			iii.	edit size		(cursorID=2
		b)	edit light				(entryID=3
			...
	2.	import/export menu			(entryID=4
		a)	import new 3D mesh		(cursorID=6
		b)	export current mesh
	3.	video and screenshot		(entryID=5
		a)	play video				(cursorID=7
		b)	capture a screenshot
	4.	Exit game
*/

void UI::pressEnter(MainGameLogic* mainGame)
{
	// entryID == 0 : main menu
	if (entryID == 0) {
		switch (entryPos[entryID]) {
		case 0: // resume game
			mainGame->closeMenu();
			break;
		case 1:	// edit my tank
			entryID = 1;
			entryPos[entryID] = 0;
			break;
		case 2: // import and export menu
			entryID = 4;
			entryPos[entryID] = 0;
			break;
		case 3: // video and screenshot
			entryID = 5;
			entryPos[entryID] = 0;
			break;
		case 4: // exit game
			glfwSetWindowShouldClose(RenderEngine::getWindow(), true); break;
		}
	}

	// entryID == 1 : edit menu
	else if (entryID == 1) {
		switch (entryPos[entryID]) {
		case 0: // edit my tank
			entryID = 2;
			entryPos[entryID] = 0;
			break;
		case 1: // edit light 
			entryID = 3;
			entryPos[entryID] = 0;
			break;
		}
	}

	// entryID == 4 : video and screenshot
	else if (entryID == 4) {
		switch (entryPos[entryID]) {
		case 0:
				// TO DO:
				// import new 3D mesh (mesh index = cursorPos[6])
			break;
		case 1:
				// TO DO:
				// export current mesh
			break;
		}
	}

	// entryID == 5 : video and screenshot
	else if (entryID == 5) {
		switch (entryPos[entryID]) {
		case 0:
				// TO DO:
				// play video (video index = cursorPos[7])
			break;
		case 1:
				// TO DO:
				// capture a screenshot
			shotflag = true;
			break;
		}
	}
}

void UI::pressEsc(MainGameLogic* mainGame)
{
	if (entryID == 0) {
		mainGame->closeMenu();
	}
	else if (entryID == 1) {
		entryID = 0;
	}
	else if (entryID == 2) {
		entryID = 1;
	}
	else if (entryID == 3) {
		entryID = 1;
	}
	else if (entryID == 4) {
		entryID = 0;
	}
	else if (entryID == 5) {
		entryID = 0;
	}
}

void UI::drawMenu()
{
	// Define the viewport dimensions : full screen
	glViewport(0, 0, RenderEngine::SCR_WIDTH, RenderEngine::SCR_HEIGHT);

	// Set OpenGL options
	//glEnable(GL_CULL_FACE);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	//glBlendFunc(GL_ONE, GL_ZERO);

	// Clear the colorbuffer
	//glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
	//glClear(GL_COLOR_BUFFER_BIT);

	switch (entryID) {
	case 0: drawMainMenu(); break;
	case 1: drawEditMenu(); break;
	case 2: drawEditTank(); break;
	case 3: drawEditLight(); break;
	case 4: drawimportMenu(); break;
	case 5: drawVideoMenu(); break;
	}
}

void UI::drawMainMenu()
{
	int myID = 0;
	drawMenuTool(myID, entryPosMax[myID], titles_main);
}

void UI::drawEditMenu()
{
	int myID = 1;
	drawMenuTool(myID, entryPosMax[myID], titles_edit);
}

void UI::drawEditTank()
{
	int myID = 2;
	char titles0[50], titles1[50], titles2[50];
	sprintf_s(titles0, 50, "edit material   %d", cursorPos[0]);
	sprintf_s(titles1, 50, "edit texture    %d", cursorPos[1]);
	sprintf_s(titles2, 50, "edit size       %d", cursorPos[2]);
	char *titles[50] = { titles0, titles1, titles2 };
	drawMenuTool(myID, entryPosMax[myID], titles);
}

void UI::drawEditLight() 
{
	int myID = 3;
	switch (cursorPos[3]) {
	case 0: cursorPosMax[4] = RenderEngine::defaultShader.dirNum; break;
	case 1: cursorPosMax[4] = RenderEngine::defaultShader.pointNum; break;
	case 2: cursorPosMax[4] = RenderEngine::defaultShader.spotNum; break;
	} 
	char *lightType[15] = { "dir light", "point light", "spot light" };
	char titles0[50]; sprintf_s(titles0, 50, "light type        %s", lightType[cursorPos[3]]);
	char titles1[50]; sprintf_s(titles1, 50, "light index       %d", cursorPos[4]);
	char titles2[50]; sprintf_s(titles2, 50, "postion x         %d", cursorPos[10]);
	char titles3[50]; sprintf_s(titles3, 50, "postion y		   %d", cursorPos[11]);
	char titles4[50]; sprintf_s(titles4, 50, "postion z         %d", cursorPos[12]);
	char titles5[50]; sprintf_s(titles5, 50, "direction x       %d/10", cursorPos[13]);
	char titles6[50]; sprintf_s(titles6, 50, "direction y       %d/10", cursorPos[14]);
	char titles7[50]; sprintf_s(titles7, 50, "direction z       %d/10", cursorPos[15]);
	char titles8[50]; sprintf_s(titles8, 50, "ambient  x        %d/10", cursorPos[16]);
	char titles9[50]; sprintf_s(titles9, 50, "ambient  y        %d/10", cursorPos[17]);
	char titles10[50]; sprintf_s(titles10, 50, "ambient  z        %d/10", cursorPos[18]);
	char titles11[50]; sprintf_s(titles11, 50, "diffuse  x        %d/10", cursorPos[19]);
	char titles12[50]; sprintf_s(titles12, 50, "diffuse  y        %d/10", cursorPos[20]);
	char titles13[50]; sprintf_s(titles13, 50, "diffuse  z        %d/10", cursorPos[21]);
	char titles14[50]; sprintf_s(titles14, 50, "specular  x       %d/10", cursorPos[22]);
	char titles15[50]; sprintf_s(titles15, 50, "specular  y       %d/10", cursorPos[23]);
	char titles16[50]; sprintf_s(titles16, 50, "specular  z       %d/10", cursorPos[24]);
	char titles17[50]; sprintf_s(titles17, 50, "constant          %d/10", cursorPos[25]);
	char titles18[50]; sprintf_s(titles18, 50, "linear            %d/100", cursorPos[26]);
	char titles19[50]; sprintf_s(titles19, 50, "quadratic         %d/1000", cursorPos[27]);

	char *titles[50] = { titles0 ,titles1,titles2,titles3,titles4,titles5,titles6,titles7,titles8,titles9, 
		titles10,titles11,titles12,titles13,titles14,titles15,titles16,titles17,titles18,titles19 };
	
	//drawMenuTool(myID, entryPosMax[myID], titles);

	glm::mat4 projection = glm::ortho(0.0f, static_cast<GLfloat>(RenderEngine::SCR_WIDTH), static_cast<GLfloat>(RenderEngine::SCR_HEIGHT), 0.0f);
	RenderEngine::characterShader.use();
	RenderEngine::characterShader.setMat4("projection", projection);

	for (int i = 0; i < 20; i++)
		UI::renderText(titles[i], RenderEngine::SCR_WIDTH / 8 * 3, RenderEngine::SCR_HEIGHT / (entryPosMax[myID] + 1) * (i + 1) - 10, 0.5);

	int i = entryPos[myID];
	drawRecP(RenderEngine::SCR_WIDTH / 2, RenderEngine::SCR_HEIGHT / (entryPosMax[myID] + 1) * (entryPosMax[myID] - i), RenderEngine::SCR_WIDTH / 2,
			RenderEngine::SCR_HEIGHT / (entryPosMax[myID] + 1), redlight);
}
void UI::drawimportMenu() 
{
	int myID = 4;
	char titles0[50];
	sprintf_s(titles0, 50, "import new 3D mesh   %d", cursorPos[6]);
	char *titles[50] = { titles0, "export current mesh" };
	drawMenuTool(myID, entryPosMax[myID], titles);
}
void UI::drawVideoMenu() 
{
	int myID = 5;
	char titles0[50];
	sprintf_s(titles0, 50, "play video     %d", cursorPos[7]);
	char *titles[50] = { titles0, "capture a screenshot" };
	drawMenuTool(myID, entryPosMax[myID], titles);
}

void UI::addEntry(int id)
{
	if (entryPos[id] + 1 < entryPosMax[id])
		entryPos[id]++;
}

void UI::subEntry(int id) {
	if (entryPos[id] - 1 >= 0)
		entryPos[id]--;
}

int UI::getCursor()
{
	int cursorID;

	if (entryID == 2 & entryPos[entryID] == 0)
		cursorID = 0;
	else if (entryID == 2 & entryPos[entryID] == 1)
		cursorID = 1;
	else if (entryID == 2 & entryPos[entryID] == 2)
		cursorID = 2;
	else if (entryID == 4 & entryPos[entryID] == 0)
		cursorID = 6;
	else if (entryID == 5 & entryPos[entryID] == 0)
		cursorID = 7;
	else if (entryID == 3) {
		if (entryPos[entryID] <= 1)
			cursorID = entryPos[entryID] + 3;
		else
			cursorID = entryPos[entryID] + 8;
	}
	else
		cursorID = -1;

	//std::cout << "cursorID = " << cursorID << std::endl;
	return cursorID;
}

void UI::addCursor()
{
	int cursorID = getCursor();
	if (cursorID < 0) return;
	if (cursorPos[cursorID] + 1 < cursorPosMax[cursorID])
		cursorPos[cursorID]++;
	if (entryID == 3)
		setNewLight();
}

void UI::subCursor()
{
	int cursorID = getCursor();
	if (cursorID < 0) return;
	if (cursorPos[cursorID] - 1 >= 0)
		cursorPos[cursorID]--;
	if (entryID == 3)
		setNewLight();
}

void UI::setNewLight()
{
	int index = cursorPos[4];
	glm::vec3 position = glm::vec3(cursorPos[10], cursorPos[11], cursorPos[12]);
	glm::vec3 direction = glm::vec3(1.0f*cursorPos[13]/cursorPosMax[13], 1.0f*cursorPos[14] / cursorPosMax[14], 1.0f*cursorPos[15] / cursorPosMax[15]);
	glm::vec3 ambient = glm::vec3(1.0*cursorPos[16] / cursorPosMax[16], 1.0*cursorPos[17] / cursorPosMax[17], 1.0*cursorPos[18] / cursorPosMax[18]);
	glm::vec3 diffuse = glm::vec3(1.0*cursorPos[19] / cursorPosMax[19], 1.0*cursorPos[20] / cursorPosMax[20], 1.0*cursorPos[21] / cursorPosMax[21]);
	glm::vec3 specular = glm::vec3(1.0*cursorPos[22] / cursorPosMax[22], 1.0*cursorPos[23] / cursorPosMax[23], 1.0*cursorPos[24] / cursorPosMax[24]);
	float constant = 1.0*cursorPos[25]/10;
	float linear = 1.0*cursorPos[26]/100;
	float quadratic = 1.0*cursorPos[27]/1000;

	switch (cursorPos[3]) {
	case 0: 
		/*std::cout << index << "," << cursorPos[13] << "," << cursorPos[14] << "," << cursorPos[15] << "," <<
			cursorPos[16] << "," << cursorPos[17] << "," << cursorPos[18] << "," <<
			cursorPos[19] << "," << cursorPos[20] << "," << cursorPos[21] << "," <<
			cursorPos[22] << "," << cursorPos[23] << "," << cursorPos[24] << std::endl;*/
		RenderEngine::setDirLight(index, direction, ambient, diffuse, specular);
		break;
	case 1:  
		RenderEngine::setPointLight(index, position, ambient, diffuse, specular, constant, linear, quadratic);
		break;
	case 2:  
		RenderEngine::setSpotLight(index, position, direction, diffuse, specular, constant, linear, quadratic, 0, 0);
		break;
	}
}

void UI::drawMenuTool(int myEntryID, int entryPosMax, char** titles)
{
	// setup the shader
	glm::mat4 projection = glm::ortho(0.0f, static_cast<GLfloat>(RenderEngine::SCR_WIDTH), static_cast<GLfloat>(RenderEngine::SCR_HEIGHT), 0.0f);
	RenderEngine::characterShader.use();
	RenderEngine::characterShader.setMat4("projection", projection);
	
	for (int i = 0; i < entryPosMax; i++)
		UI::renderText(titles[i], RenderEngine::SCR_WIDTH / 8 * 3, RenderEngine::SCR_HEIGHT / (entryPosMax + 1) * (i + 1)-10, 0.5);

	for (int i = 0; i < entryPosMax; i++)
		drawRecP(RenderEngine::SCR_WIDTH / 2, RenderEngine::SCR_HEIGHT / (entryPosMax + 1) * (entryPosMax - i), RenderEngine::SCR_WIDTH / 2,
			RenderEngine::SCR_HEIGHT / (entryPosMax + 1), entryPos[myEntryID] == i ? highlight : lowlight);
}

void UI::drawRecP(GLfloat x, GLfloat y, GLfloat w, GLfloat h, glm::vec4 RGBA)
{
	float SCR_WIDTH = static_cast<GLfloat>(RenderEngine::SCR_WIDTH);
	float SCR_HEIGHT = static_cast<GLfloat>(RenderEngine::SCR_HEIGHT);
	float centerX = (x / SCR_WIDTH - 0.5) * 2;
	float centerY = (y / SCR_HEIGHT - 0.5) * 2;
	float width = w / SCR_WIDTH;
	float height = h / SCR_WIDTH;

	drawRec(centerX, centerY, width, height, RGBA);
}

void UI::drawRec(GLfloat x, GLfloat y, GLfloat w, GLfloat h, glm::vec4 RGBA)
{
	// set up vertex data (and buffer(s)) and configure vertex attributes
	// ------------------------------------------------------------------
	float vertices[] = {
		x + w, y + h, 0.0f,  // top right
		x + w, y - h, 0.0f,  // bottom right
		x - w, y - h, 0.0f,  // bottom left
		x - w, y + h, 0.0f,   // top left 
		x + w, y + h, 0.0f,  // top right
	};
	unsigned int indices[] = {  // note that we start from 0!
		0, 1, 2, 3, 4
	};
	unsigned int VBO, VAO, EBO;
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);
	// bind the Vertex Array Object first, then bind and set vertex buffer(s), and then configure vertex attributes(s).
	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	// note that this is allowed, the call to glVertexAttribPointer registered VBO as the vertex attribute's bound vertex buffer object so afterwards we can safely unbind
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	// remember: do NOT unbind the EBO while a VAO is active as the bound element buffer object IS stored in the VAO; keep the EBO bound.
	//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	// You can unbind the VAO afterwards so other VAO calls won't accidentally modify this VAO, but this rarely happens. Modifying other
	// VAOs requires a call to glBindVertexArray anyways so we generally don't unbind VAOs (nor VBOs) when it's not directly necessary.
	glBindVertexArray(0);

	glm::mat4 projection = glm::ortho(0.0f, static_cast<GLfloat>(RenderEngine::SCR_WIDTH), static_cast<GLfloat>(RenderEngine::SCR_HEIGHT), 0.0f);
	RenderEngine::twoDShader.use();
	RenderEngine::twoDShader.setVec4("RGBA", RGBA);
	glBindVertexArray(VAO); // seeing as we only have a single VAO there's no need to bind it every time, but we'll do so to keep things a bit more organized
							//glDrawArrays(GL_TRIANGLES, 0, 6);
							//glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 5);
	glBindVertexArray(0); // no need to unbind it every time 
}

void UI::init()
{
	// FreeType
	FT_Library ft;
	// All functions return a value different than 0 whenever an error occurred
	if (FT_Init_FreeType(&ft))
		std::cout << "ERROR::FREETYPE: Could not init FreeType Library" << std::endl;

	// Load font as face
	FT_Face face;
	if (FT_New_Face(ft, "fonts/arial.ttf", 0, &face))
		std::cout << "ERROR::FREETYPE: Failed to load font" << std::endl;

	// Set size to load glyphs as
	FT_Set_Pixel_Sizes(face, 0, 48);

	// Disable byte-alignment restriction
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

	// Load first 128 characters of ASCII set
	for (GLubyte c = 0; c < 128; c++) {
		// Load character glyph 
		if (FT_Load_Char(face, c, FT_LOAD_RENDER)) {
			std::cout << "ERROR::FREETYTPE: Failed to load Glyph" << std::endl;
			continue;
		}
		// Generate texture
		GLuint texture;
		glGenTextures(1, &texture);
		glBindTexture(GL_TEXTURE_2D, texture);
		glTexImage2D(
			GL_TEXTURE_2D,
			0,
			GL_RED,
			face->glyph->bitmap.width,
			face->glyph->bitmap.rows,
			0,
			GL_RED,
			GL_UNSIGNED_BYTE,
			face->glyph->bitmap.buffer
		);
		// Set texture options
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		// Now store character for later use
		Character character = {
			texture,
			glm::ivec2(face->glyph->bitmap.width, face->glyph->bitmap.rows),
			glm::ivec2(face->glyph->bitmap_left, face->glyph->bitmap_top),
			face->glyph->advance.x
		};
		Characters.insert(std::pair<GLchar, Character>(c, character));
	}
	glBindTexture(GL_TEXTURE_2D, 0);
	// Destroy FreeType once we're finished
	FT_Done_Face(face);
	FT_Done_FreeType(ft);

	// Configure VAO/VBO for texture quads
	glGenVertexArrays(1, &UI_VAO);
	glGenBuffers(1, &UI_VBO);
	glBindVertexArray(UI_VAO);
	glBindBuffer(GL_ARRAY_BUFFER, UI_VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 6 * 4, NULL, GL_DYNAMIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), 0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void UI::renderText(std::string text, GLfloat x, GLfloat y, GLfloat scale, glm::vec3 color)
{
	UI::RenderText(RenderEngine::characterShader, text, x, y, scale, color);
}

void UI::RenderText(Shader &shader, std::string text, GLfloat x, GLfloat y, GLfloat scale, glm::vec3 color)
{
	// Activate corresponding render state	 
	shader.use();
	shader.setVec3("textColor", color);
	glActiveTexture(GL_TEXTURE0);
	glBindVertexArray(UI_VAO);

	// Iterate through all characters
	std::string::const_iterator c;
	for (c = text.begin(); c != text.end(); c++) {
		Character ch = Characters[*c];

		GLfloat xpos = x + ch.Bearing.x * scale;
		GLfloat ypos = y + (Characters['H'].Bearing.y - ch.Bearing.y) * scale;

		GLfloat w = ch.Size.x * scale;
		GLfloat h = ch.Size.y * scale;
		// 为每个字符更新VBO
		GLfloat vertices[6][4] = {
			{ xpos,     ypos + h,   0.0, 1.0 },
			{ xpos + w, ypos,       1.0, 0.0 },
			{ xpos,     ypos,       0.0, 0.0 },

			{ xpos,     ypos + h,   0.0, 1.0 },
			{ xpos + w, ypos + h,   1.0, 1.0 },
			{ xpos + w, ypos,       1.0, 0.0 }
		};

		// Render glyph texture over quad
		glBindTexture(GL_TEXTURE_2D, ch.TextureID);
		// Update content of VBO memory
		glBindBuffer(GL_ARRAY_BUFFER, UI_VBO);
		glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices); // Be sure to use glBufferSubData and not glBufferData

		glBindBuffer(GL_ARRAY_BUFFER, 0);
		// Render quad
		glDrawArrays(GL_TRIANGLES, 0, 6);
		// Now advance cursors for next glyph (note that advance is number of 1/64 pixels)
		x += (ch.Advance >> 6) * scale; // Bitshift by 6 to get value in pixels (2^6 = 64 (divide amount of 1/64th pixels by 64 to get amount of pixels))
	}
	glBindVertexArray(0);
	glBindTexture(GL_TEXTURE_2D, 0);
}

float UI::blood = 90.0f;

void UI::drawBlood()
{
	glViewport(0, 0, RenderEngine::SCR_WIDTH, RenderEngine::SCR_HEIGHT);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	// blood rec: 100*20
	float width_1 = blood;
	float width_2 = 100.0f - blood;

	drawRecP(15+width_1/2, 30, width_1, 30, glm::vec4(1.0, 0.0f, 0.0f, 0.5f));
	drawRecP(15+width_1+width_2/2, 30, width_2, 30, glm::vec4(0.8f, 0.8f, 0.8f, 0.5f));
}

float UI::getBlood()
{
	return blood;
}

void UI::setBlood(float newBlood)
{
	blood = newBlood;
}

void UI::addBlood(float delta)
{
	blood += delta;
	if (blood > 100) blood = 100;
}

void UI::subBlood(float delta)
{
	blood -= delta;
	if (blood < 0) blood = 0;
}

bool UI::shotflag = false;
#define BMP_Header_Length 54

void UI::screenshot()
{
	// if no screenshot command from user
	if (!shotflag)
		return;
	
	shotflag = false;

	FILE*    pDummyFile;  //指向另一bmp文件，用于复制它的文件头和信息头数据
	FILE*    pWritingFile;  //指向要保存截图的bmp文件
	GLubyte* pPixelData;    //指向新的空的内存，用于保存截图bmp文件数据
	GLubyte  BMP_Header[BMP_Header_Length];
	GLint    i, j;
	GLint    PixelDataLength;   //BMP文件数据总长度

	GLint WindowWidth = RenderEngine::SCR_WIDTH;
	GLint WindowHeight = RenderEngine::SCR_HEIGHT;
								// 计算像素数据的实际长度
	i = RenderEngine::SCR_WIDTH * 3;   // 得到每一行的像素数据长度
	while (i % 4 != 0)      // 补充数据，直到i是4的倍数
		++i;
	PixelDataLength = i * RenderEngine::SCR_HEIGHT;  //补齐后的总位数

										 // 分配内存和打开文件
	pPixelData = (GLubyte*)malloc(PixelDataLength);
	if (pPixelData == 0)
		exit(0);

	fopen_s(&pDummyFile, "res\\screenshot\\bitmap1.bmp", "rb");//只读形式打开
	if (pDummyFile == 0)
		exit(0);

	fopen_s(&pWritingFile, "res\\screenshot\\grab.bmp", "wb"); //只写形式打开
	if (pWritingFile == 0)
		exit(0);

	//把读入的bmp文件的文件头和信息头数据复制，并修改宽高数据
	fread(BMP_Header, sizeof(BMP_Header), 1, pDummyFile);  //读取文件头和信息头，占据54字节
	fwrite(BMP_Header, sizeof(BMP_Header), 1, pWritingFile);
	fseek(pWritingFile, 0x0012, SEEK_SET); //移动到0X0012处，指向图像宽度所在内存
	i = WindowWidth;
	j = WindowHeight;
	fwrite(&i, sizeof(i), 1, pWritingFile);
	fwrite(&j, sizeof(j), 1, pWritingFile);

	// 读取当前画板上图像的像素数据
	glPixelStorei(GL_UNPACK_ALIGNMENT, 4);  //设置4位对齐方式
	glReadPixels(0, 0, WindowWidth, WindowHeight,
		GL_BGR, GL_UNSIGNED_BYTE, pPixelData);

	// 写入像素数据
	fseek(pWritingFile, 0, SEEK_END);
	//把完整的BMP文件数据写入pWritingFile
	fwrite(pPixelData, PixelDataLength, 1, pWritingFile);

	// 释放内存和关闭文件
	fclose(pDummyFile);
	fclose(pWritingFile);
	free(pPixelData);
}
