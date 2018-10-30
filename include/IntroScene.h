#pragma once
#include <Scene.h>

class IntroScene : public Scene
{
public:

	void Enter();
	void Exit();
	SceneIdentifier Update(float ElapsedTime, Input& InputManager, Renderer& Render);
private:

};