
#include <IntroScene.h>
#include <string>

void IntroScene::Enter()
{
}

void IntroScene::Exit()
{
}

SceneIdentifier IntroScene::Update(float ElapsedTime)
{
	if (GetEngine()->Input.GetDown(EInputKeys::START))
		return SceneIdentifier::GAME;

	return SceneIdentifier::INTRO;
}
