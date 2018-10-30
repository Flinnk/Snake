#include <IntroScene.h>

void IntroScene::Enter()
{

}

void IntroScene::Exit()
{

}

SceneIdentifier IntroScene::Update(float ElapsedTime, Input& InputManager, Renderer& Render)
{
	if(InputManager.GetDown(InputKeys::START))
		return SceneIdentifier::GAME;

	return SceneIdentifier::INTRO;
}
