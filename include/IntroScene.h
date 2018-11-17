#pragma once
#include <Scene.h>

class IntroScene : public Scene
{
public:

	void Enter();
	void Exit();
	SceneIdentifier Update(float ElapsedTime);
private:
};