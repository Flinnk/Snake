
#include <IntroScene.h>
#include <string>

void IntroScene::Enter()
{
	TitleAnimation.Initialize(0, WINDOW_HEIGHT - 20, 3.0f);
	AlphaAnimation.Initialize(0, 1.0f, 0.25f);

}

void IntroScene::Exit()
{
}

SceneIdentifier IntroScene::Update(float ElapsedTime)
{
	if (GetEngine()->Input.GetDown(EInputKeys::START))
		return SceneIdentifier::GAME;

	float Position = 0.0f;
	float Alpha = 0.0f;
	TitleAnimation.Run(ElapsedTime,&Position);
	if (AlphaAnimation.Run(ElapsedTime, &Alpha))
	{
		AlphaAnimation.Reset();
		AlphaAnimation.Inverse();
	}
	GetEngine()->Renderer.DrawTextExt(GetEngine()->ResourceManager.RetrieveFont("Boxy-Bold.ttf"), "Smooth transition", Vector3(0, Position, 0), Vector3(20, 20, 0), Vector3(0, 0, 0), Vector4(1, 1, 1, Alpha));

	return SceneIdentifier::INTRO;
}
