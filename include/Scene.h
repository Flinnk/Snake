#pragma once
#include "SnakeRenderer.h"
#include "SnakeInput.h"

enum class SceneIdentifier : unsigned int
{
	INTRO,
	GAME
};

class Scene
{
public:
	virtual void Enter(Renderer& Render) = 0;
	virtual void Exit(Renderer& Render) = 0;
	virtual SceneIdentifier Update(float ElapsedTime,Input& InputManager, Renderer& Render) = 0;
};