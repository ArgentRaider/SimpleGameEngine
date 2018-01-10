#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include <iostream>
#include <string>
#include <RenderEngine/RenderEngine.h>
#include <GameLogic/GameLogic.h>


int main() {
	if (RenderEngine::init("Simple Test")) {
		MainGameLogic mainGameLogic;

		RenderEngine::setGameLogic(mainGameLogic);

		RenderEngine::run();
	}
	RenderEngine::stop();

	return 0;
}