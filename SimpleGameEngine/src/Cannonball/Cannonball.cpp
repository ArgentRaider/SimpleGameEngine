#include "Cannonball.h"
#include <RenderEngine/RenderEngine.h>

Cannonball::Cannonball()
{
	init();
}

void Cannonball::init()
{
	if (model != NULL)
	{
		RenderEngine::deleteModel(*model);
	}
	model = NULL;
	v = { 0.0f, 0.0f, 0.0f };
	pos = { 0.0f, 0.0f, 0.0f };
}

void Cannonball::set(Model* model, glm::vec3 v0, glm::vec3 pos0)
{
	this->v = v0;
	this->pos = pos0;
	
	model->Translate(pos);
	this->model = model;
	RenderEngine::addModel(*model);
}

void Cannonball::Render()
{
	if (model != NULL)
	{
		if (RenderEngine::existCollision(model))
		{
			RenderEngine::particleSystem.initParticles(pos);
			init();
			if (MainGameLogic::turnState == 0 || MainGameLogic::turnState == 1)
				UI::subBlood(2, 20.0f);
			else if (MainGameLogic::turnState == 2 || MainGameLogic::turnState == 3)
				UI::subBlood(1, 20.0f);
			MainGameLogic::attackFinished = true;
		}
		else if (pos.y <= RenderEngine::getHeight(pos.x, pos.z) + 0.1f)
		{
			RenderEngine::particleSystem.initParticles(pos);
			init();
			MainGameLogic::attackFinished = true;
		
		}
		else
		{
			float deltaTime = 0.017f;
			glm::vec3 DeltaP = v*deltaTime;
			glm::vec3 DeltaV = glm::vec3(0.0f, -9.81f, 0.0f)*deltaTime;
			pos += DeltaP;
			v += DeltaV;
			model->Translate(DeltaP);
		}
	}
}