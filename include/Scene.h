#pragma once
#include <Engine.h>

enum class SceneIdentifier : unsigned int
{
	INTRO,
	GAME
};

class Scene
{
public:
	virtual void Enter() = 0;
	virtual void Exit() = 0;
	virtual SceneIdentifier Update(float ElapsedTime) = 0;
};