
#include <IntroScene.h>
#include <string>
#include <Audio.h>

void IntroScene::Enter()
{
	TitleAnimation.Initialize(WINDOW_HEIGHT, WINDOW_HEIGHT / 2 + 60 * 2, 1.0f);
	AlphaAnimation.Initialize(0, 1.0f, 0.4f);
	CAudioClip* AudioClip = GetEngine()->ResourceManager.LoadAndRetrieveAudioClip("trance-menu.wav", true);
	AudioClip->Play();
	AudioClip->SetVolume(0.05f);
	Font = GetEngine()->ResourceManager.RetrieveFont("Boxy-Bold.ttf");
	CurrentButton = IntroButton::PLAY;
}

void IntroScene::Exit()
{
	GetEngine()->ResourceManager.ReleaseAudioClip("trance-menu.wav");
}

SceneIdentifier IntroScene::Update(float ElapsedTime)
{
	CEngine* Engine = GetEngine();
	float Position = 0.0f;
	bool TitleAnimationFinished = TitleAnimation.Run(ElapsedTime, &Position);
	if (GetEngine()->Input.GetDown(EInputKeys::START))
	{
		if(CurrentButton == IntroButton::PLAY)
		{
			return SceneIdentifier::GAME;
		}
		else
		{
			Engine->bClose = true;
			return SceneIdentifier::INTRO;
		}
	}

	unsigned int ButtonValue = (unsigned int)CurrentButton;
	if (Engine->Input.GetDown(EInputKeys::UP))
	{
		ButtonValue = min(ButtonValue - 1, (unsigned int)IntroButton::PLAY);
	}
	else if (Engine->Input.GetDown(EInputKeys::DOWN))
	{
		ButtonValue = min(ButtonValue + 1, (unsigned int)IntroButton::EXIT);
	}
	CurrentButton = (IntroButton)ButtonValue;

	float Alpha = 0.0f;
	if (AlphaAnimation.Run(ElapsedTime, &Alpha))
	{
		AlphaAnimation.Reset();
		AlphaAnimation.Inverse();
	}
	Engine->Renderer.DrawTextExt(Font, "Snake", Vector3(WINDOW_WIDTH / 2 - 5 * 60 / 2, Position, 0), Vector3(60, 60, 0), Vector3(0, 0, 0), Vector4(0, 1, 0, 1));

	if (TitleAnimationFinished)
	{
		Engine->Renderer.DrawTextExt(Font, "Play", Vector3(WINDOW_WIDTH / 2 - 4 * 30 / 2, WINDOW_HEIGHT / 2 - 60, 0), Vector3(30, 30, 0), Vector3(0, 0, 0), Vector4(0, 1, 0, CurrentButton == IntroButton::PLAY ? Alpha : 1));
		Engine->Renderer.DrawTextExt(Font, "Exit", Vector3(WINDOW_WIDTH / 2 - 4 * 30 / 2, WINDOW_HEIGHT / 2 - 120, 0), Vector3(30, 30, 0), Vector3(0, 0, 0), Vector4(0, 1, 0, CurrentButton == IntroButton::EXIT ? Alpha : 1));
	}

	return SceneIdentifier::INTRO;
}
