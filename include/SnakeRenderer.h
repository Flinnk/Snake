#pragma once
#include <Windows.h>
#include <SnakeMath.h>
#include <d3d11.h>

struct VertexData
{
	Vector3 Position;
	Vector4 Color;
};

struct ConstantBufferData
{
	Matrix4x4 MVP;
};

class Renderer
{
public:
	Renderer() : Width(0), Height(0) {}
	bool Initialize(HWND WindowHandle, int Width, int Height);

	void Clear(const Vector4& Color);
	void DrawRectangle(Vector3 Position, Vector3 Scale);
	void Present();

	void Release();
private:
	int Width, Height;
	D3D_FEATURE_LEVEL FeatureLevel;
	ID3D11Device* Device = nullptr;
	ID3D11DeviceContext* DeviceContext = nullptr;
	IDXGISwapChain* SwapChain = nullptr;
	ID3D11RenderTargetView* RenderTargetView = nullptr;
	ID3D11DepthStencilView* DepthStencilView = nullptr;
	ID3D11Texture2D* DepthStencilTexture = nullptr;
	ID3D11VertexShader* VertexShader = nullptr;
	ID3D11PixelShader* PixelShader = nullptr;
	ID3D11InputLayout* InputLayout = nullptr;
	ID3D11Buffer* ConstantBuffer = nullptr;
	ID3D11Buffer* VertexBuffer = nullptr;
	ID3D11Buffer* IndexBuffer = nullptr;
};