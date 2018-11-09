#include <IntroScene.h>

void IntroScene::Enter(Renderer& Render)
{
	TestSprite = Render.LoadTextureFromFile("tumblr_nzbtsemq3e1syry3co5_540.png");
}

void IntroScene::Exit(Renderer& Render)
{
	TestSprite.Release();
}

SceneIdentifier IntroScene::Update(float ElapsedTime, Input& InputManager, Renderer& Render)
{
	if(InputManager.GetDown(InputKeys::START))
		return SceneIdentifier::GAME;

	Render.DrawSprite(Vector3(0, 0, 0), Vector3(800, 600, 0), Vector3(0, 0, 0), Vector4(1, 1, 1, 1),&TestSprite);

	return SceneIdentifier::INTRO;
}
