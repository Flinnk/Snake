#pragma once
#include <MathExt.h>
#include <d3d11.h>
#include <stb/stb_truetype.h>

#define D3D_SAFE_RELEASE(x) \
if(x)\
{\
	x->Release();\
	x = nullptr;\
}\

#define NUM_RECT_BATCH 250
#define VERTEX_PER_RECT 4
#define INDICES_PER_RECT 6
#define NUM_VERTICES (NUM_RECT_BATCH * VERTEX_PER_RECT)
#define NUM_INDICES (NUM_RECT_BATCH * INDICES_PER_RECT)

struct SVertexData
{
	Vector3 Position;
	Vector4 Color;
	Vector3 UV;
};

struct SConstantBufferData
{
	Matrix4x4 MVP;
};

class CTexture
{
	friend class CRenderer;
public:
	CTexture() : RawTexture(nullptr), TextureView(nullptr), Width(0), Height(0)
	{

	}

	CTexture(ID3D11Texture2D* TexturePointer, ID3D11ShaderResourceView* ResourcePointer, int TextureWidth, int TextureHeight) : RawTexture(TexturePointer), TextureView(ResourcePointer), Width(TextureWidth), Height(TextureHeight)
	{
	}

	void Release()
	{
		D3D_SAFE_RELEASE(RawTexture);
		D3D_SAFE_RELEASE(TextureView);
	}

	int GetWidth()
	{
		return Width;
	}
	int GetHeight()
	{
		return Height;
	}

private:

	int Width;
	int Height;
	ID3D11Texture2D* RawTexture = nullptr;
	ID3D11ShaderResourceView* TextureView = nullptr;
};

class CFont
{
	friend class CRenderer;
public:
	CFont() : Texture(nullptr)
	{

	}

	void Release()
	{
		if (Texture)
			Texture->Release();
		delete Texture;
		Texture = nullptr;
	}

private:
	CTexture* Texture = nullptr;
	stbtt_bakedchar cdata[255];
};

enum class ERenderMode
{
	NONE,
	SPRITE,
	TEXT
};

class CRenderer
{
public:
	CRenderer() : Width(0), Height(0) {}
	bool Initialize(HWND WindowHandle, int Width, int Height);

	void Clear(const Vector4& Color);
	void Begin();

	CTexture LoadTextureFromFile(const char* Path);
	CTexture* LoadTextureFromMemory(const unsigned char* Data, int TextureWidth, int TextureHeight, int TextureChannels);
	CFont LoadFont(const char* Path, int Size, int BitFontWidth, int BitFontHeight);
	void DrawSprite(Vector3 Position, Vector3 Size, Vector3 Offset, Vector4 Color, CTexture* SpriteTexture = nullptr, Vector3 UVPos = Vector3(0, 0, 0), Vector3 UVSize = Vector3(1, 1, 0));
	void DrawTextExt(CFont* Font, const char* Text, Vector3 Position, Vector3 Size, Vector3 Offset, Vector4 Color);
	void End();

	void Present();

	void Release();
private:

	void InternalDrawSprite(ERenderMode RenderMode, Vector3 Position, Vector3 Size, Vector3 Offset, Vector4 Color, CTexture* SpriteTexture = nullptr, Vector3 UVPos = Vector3(0, 0, 0), Vector3 UVSize = Vector3(1, 1, 0));

	int Width, Height;
	D3D_FEATURE_LEVEL FeatureLevel;
	ID3D11Device* Device = nullptr;
	ID3D11DeviceContext* DeviceContext = nullptr;
	IDXGISwapChain* SwapChain = nullptr;
	ID3D11RenderTargetView* RenderTargetView = nullptr;
	ID3D11DepthStencilView* DepthStencilView = nullptr;
	ID3D11Texture2D* DepthStencilTexture = nullptr;

	ID3D11VertexShader* SpriteVertexShader = nullptr;
	ID3D11PixelShader* SpritePixelShader = nullptr;
	ID3D11VertexShader* TextVertexShader = nullptr;
	ID3D11PixelShader* TextPixelShader = nullptr;

	ID3D11InputLayout* InputLayout = nullptr;

	ID3D11Buffer* ConstantBuffer = nullptr;
	ID3D11Buffer* VertexBuffer = nullptr;
	ID3D11Buffer* IndexBuffer = nullptr;
	SVertexData* VertexBufferPointer = nullptr;
	ID3D11SamplerState* SamplerState = nullptr;
	ID3D11BlendState* BlendState = nullptr;

	CTexture SpriteTexture;
	CTexture* LastDrawnTexture = nullptr;
	unsigned int IndicesToDraw = 0;
	ERenderMode CurrentRenderMode = ERenderMode::NONE;
};