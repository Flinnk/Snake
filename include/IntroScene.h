#pragma once
#include <Scene.h>
#include <Constants.h>
#include <Transition.h>

enum class IntroButton : unsigned int
{
	PLAY,
	EXIT
};

class CFont;
class IntroScene : public Scene
{
public:

	void Enter();
	void Exit();
	SceneIdentifier Update(float ElapsedTime);
private:
	Transition<float> TitleAnimation;
	Transition<float> AlphaAnimation;
	CFont* Font;
	IntroButton CurrentButton;
};