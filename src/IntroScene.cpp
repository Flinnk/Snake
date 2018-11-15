
#include <IntroScene.h>
#include <string>

void IntroScene::Enter()
{
}

void IntroScene::Exit()
{
	TestSprite.Release();
}

SceneIdentifier IntroScene::Update(float ElapsedTime)
{
	if (GetEngine()->Input.GetDown(EInputKeys::START))
		return SceneIdentifier::GAME;

	return SceneIdentifier::INTRO;
}
