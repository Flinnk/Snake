#pragma warning(disable:4996)

#include <IntroScene.h>
#include <stb/stb_truetype.h>
#include <string>

stbtt_bakedchar cdata[255]; // ASCII 32..126 is 95 glyphs
void IntroScene::Enter()
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

	int result = stbtt_BakeFontBitmap(fontBuffer, 0, 48, temp_bitmap, 512, 512, 0, 255, cdata); // no guarantee this fits!

	TestSprite = GetEngine()->Renderer.LoadTextureFromMemory(temp_bitmap, 512, 512, 1);
	free(fontBuffer);

	//TestSprite = Render.LoadTextureFromFile("tumblr_nzbtsemq3e1syry3co5_540.png");
}

void IntroScene::Exit()
{
	TestSprite.Release();
}

SceneIdentifier IntroScene::Update(float ElapsedTime)
{
	if (GetEngine()->Input.GetDown(EInputKeys::START))
		return SceneIdentifier::GAME;

	//Render.DrawSprite(Vector3(0, 0, 0), Vector3(800, 600, 0), Vector3(0, 0, 0), Vector4(1, 1, 1, 1), &TestSprite);
	const char* SampleText = "Snake";
	int xOffset = 0;
	for (int i = 0; i < strlen(SampleText); ++i)
	{
		Vector3 Size(50, 50, 0);
		const char Character = SampleText[i];
		if (Character != ' ')
		{
			stbtt_bakedchar CharInfo = cdata[Character];
			Vector3 UVPos(CharInfo.x0 / 512.0, CharInfo.y0 / 512.0, 0);
			Vector3 UVSize(CharInfo.x1 / 512.0, CharInfo.y1 / 512.0, 0);
			GetEngine()->Renderer.DrawSprite(Vector3(0 + xOffset, 0, 0), Size, Vector3(0, 0, 0), Vector4(1, 1, 1, 1), &TestSprite, UVPos, UVSize);
			xOffset += Size.X;
		}
		else
		{
			xOffset += Size.X;
		}
	}

	return SceneIdentifier::INTRO;
}
