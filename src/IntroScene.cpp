
#include <IntroScene.h>
#include <string>

void IntroScene::Enter()
{
	TitleAnimation.Initialize(WINDOW_HEIGHT, WINDOW_HEIGHT / 2 + 60 * 2, 1.0f);
	AlphaAnimation.Initialize(0, 1.0f, 0.4f);
	Music = new(MemoryManager.AllocateSceneMemory(sizeof(CAudioClip)))CAudioClip();
	AudioManager.LoadAudio(MemoryManager.GetSceneMemoryAllocator(), "trance-menu.wav", Music, true);
	Music->Play();
	Music->SetVolume(0.05f);
	CurrentButton = IntroButton::PLAY;
}

void IntroScene::Exit()
{
	Music->Stop();
}

SceneIdentifier IntroScene::Update(float ElapsedTime)
{
	CEngine* Engine = GetEngine();
	float Position = 0.0f;
	bool TitleAnimationFinished = TitleAnimation.Run(ElapsedTime, &Position);
	if (InputManager.GetDown(EInputKeys::START) && TitleAnimationFinished)
	{
		if (CurrentButton == IntroButton::PLAY)
		{
			return SceneIdentifier::GAME;
		}
		else
		{
			Engine->RequestExit();
			return SceneIdentifier::INTRO;
		}
	}

	unsigned int ButtonValue = (unsigned int)CurrentButton;
	if (InputManager.GetDown(EInputKeys::UP))
	{
		ButtonValue = min(ButtonValue - 1, (unsigned int)IntroButton::PLAY);
	}
	else if (InputManager.GetDown(EInputKeys::DOWN))
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
	Renderer.DrawTextExt("Snake", Vector3(WINDOW_WIDTH / 2 - 5 * 60 / 2, Position, 0), Vector3(60, 60, 0), Vector3(0, 0, 0), Vector4(0, 1, 0, 1));

	if (TitleAnimationFinished)
	{
		Renderer.DrawTextExt("Play", Vector3(WINDOW_WIDTH / 2 - 4 * 30 / 2, WINDOW_HEIGHT / 2 - 60, 0), Vector3(30, 30, 0), Vector3(0, 0, 0), Vector4(0, 1, 0, CurrentButton == IntroButton::PLAY ? Alpha : 1));
		Renderer.DrawTextExt("Exit", Vector3(WINDOW_WIDTH / 2 - 4 * 30 / 2, WINDOW_HEIGHT / 2 - 120, 0), Vector3(30, 30, 0), Vector3(0, 0, 0), Vector4(0, 1, 0, CurrentButton == IntroButton::EXIT ? Alpha : 1));
	}

	return SceneIdentifier::INTRO;
}
