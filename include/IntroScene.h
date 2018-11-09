#pragma once
#include <Scene.h>

class IntroScene : public Scene
{
public:

	void Enter(Renderer& Render);
	void Exit(Renderer& Render);
	SceneIdentifier Update(float ElapsedTime, Input& InputManager, Renderer& Render);
private:
	Texture TestSprite;
};