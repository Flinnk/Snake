#pragma warning(disable:4996)

#include <IntroScene.h>
#include <stb/stb_truetype.h>
#include <string>

void IntroScene::Enter(Renderer& Render)
{

	long size;
	unsigned char* fontBuffer;
	FILE* fontFile = fopen("Boxy-Bold.ttf", "rb");
	fseek(fontFile, 0, SEEK_END);
	size = ftell(fontFile); /* how long is the file ? */
	fseek(fontFile, 0, SEEK_SET); /* reset */

	fontBuffer = (unsigned char*)malloc(size);

	fread(fontBuffer, size, 1, fontFile);
	fclose(fontFile);

	unsigned char temp_bitmap[512 * 512];
	stbtt_bakedchar cdata[96]; // ASCII 32..126 is 95 glyphs

	int result = stbtt_BakeFontBitmap(fontBuffer, 0, 58, temp_bitmap, 512, 512, 32, 96, cdata); // no guarantee this fits!

	TestSprite = Render.LoadTextureFromMemory(temp_bitmap, 512, 512, 1);
	free(fontBuffer);

	//TestSprite = Render.LoadTextureFromFile("tumblr_nzbtsemq3e1syry3co5_540.png");
}

void IntroScene::Exit(Renderer& Render)
{
	TestSprite.Release();
}

SceneIdentifier IntroScene::Update(float ElapsedTime, Input& InputManager, Renderer& Render)
{
	if (InputManager.GetDown(InputKeys::START))
		return SceneIdentifier::GAME;

	Render.DrawSprite(Vector3(0, 0, 0), Vector3(800, 600, 0), Vector3(0, 0, 0), Vector4(1, 1, 1, 1), &TestSprite);

	return SceneIdentifier::INTRO;
}
