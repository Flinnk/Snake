#pragma once
#include <Scene.h>
#include <Constants.h>
#include <Transition.h>

class IntroScene : public Scene
{
public:

	void Enter();
	void Exit();
	SceneIdentifier Update(float ElapsedTime);
private:
	Transition<float> TitleAnimation;
	Transition<float> AlphaAnimation;
};