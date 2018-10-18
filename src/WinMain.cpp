#include <Windows.h>
#include <d3d11.h>
#include <string>
#include <d3dcompiler.h>
#include <DirectXMath.h>

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
void ShowSystemErrorMessage(const char* Message);

static bool bRun = true;


static const char ShaderCode[] = "\
	cbuffer BufferPerBatch  \
	{	float4x4 Projection;\
	}\
\
	struct VertexInput\
{\
	float4 Position : POSITION;\
	float4 Color : COLOR;\
};\
\
struct PixelInput\
{\
	float4 Position : SV_POSITION;\
	float4 Color : COLOR;\
};\
\
PixelInput VS(VertexInput Input)\
{\
	PixelInput Output;\
	Input.Position.w = 1;\
	Output.Color = Input.Color;\
	Output.Position = mul(Input.Position, Projection);\
	return Output;\
}\
\
float4 PS(PixelInput Input) : SV_TARGET\
{\
	return Input.Color;\
}\
";

struct Vector3
{
	union
	{
		struct { float X, Y, Z; };
		float Components[3];
	};

	Vector3() :X(0), Y(0), Z(0) {}

	Vector3(float x, float y, float z) :X(x), Y(y), Z(z) {}
};

struct Vector4
{
	union
	{
		struct { float X, Y, Z, W; };
		float Components[4];
	};


	Vector4() :X(0), Y(0), Z(0), W(0) {}

	Vector4(float x, float y, float z, float w) :X(x), Y(y), Z(z), W(w) {}

	Vector4& operator=(const Vector4& Other)
	{
		X = Other.X;
		Y = Other.Y;
		Z = Other.Z;
		W = Other.W;
		return *this;
	}
};

struct Matrix4x4
{
	union
	{
		float Elements[16];
	};

	Matrix4x4(float Diagonal)
	{
		for (int i = 0; i < 4 * 4; ++i) {
			Elements[i] = 0;
		}

		Elements[0] = Diagonal;
		Elements[5] = Diagonal;
		Elements[10] = Diagonal;
		Elements[15] = Diagonal;
	}

	Matrix4x4()
	{
		for (int i = 0; i < 4 * 4; ++i) {
			Elements[i] = 0;
		}
	}

	static Matrix4x4 Identity()
	{
		return Matrix4x4(1.0f);
	}

	static Matrix4x4 Translation(float X, float Y, float Z)
	{
		Matrix4x4 Result(1.0f);

		Result.Elements[3] = X;
		Result.Elements[7] = Y;
		Result.Elements[11] = Z;

		return Result;
	}

	static Matrix4x4 Scaling(float X, float Y, float Z)
	{
		Matrix4x4 Result(1.0f);

		Result.Elements[0] = X;
		Result.Elements[5] = Y;
		Result.Elements[10] = Z;

		return Result;
	}

	static Matrix4x4 Ortographic(float Width,float Height, float NearPlane, float FarPlane)
	{
		Matrix4x4 Result(1.0f);

		float fRange = 1.0f / (FarPlane - NearPlane);

		Result.Elements[0] = 2 / Width;
		Result.Elements[5] = 2 / Height;
		Result.Elements[10] = fRange;

		Result.Elements[7] = -fRange * NearPlane;
		return Result;
	}

	Matrix4x4 Multiply(const Matrix4x4& Left, const Matrix4x4& Right)
	{
		Matrix4x4 Result;

		Result.Elements[0] = Right.Elements[0] * Left.Elements[0] + Right.Elements[1] * Left.Elements[4] + Right.Elements[2] * Left.Elements[8] + Right.Elements[3] * Left.Elements[12];
		Result.Elements[1] = Right.Elements[0] * Left.Elements[1] + Right.Elements[1] * Left.Elements[5] + Right.Elements[2] * Left.Elements[9] + Right.Elements[3] * Left.Elements[13];
		Result.Elements[2] = Right.Elements[0] * Left.Elements[2] + Right.Elements[1] * Left.Elements[6] + Right.Elements[2] * Left.Elements[10] + Right.Elements[3] * Left.Elements[14];
		Result.Elements[3] = Right.Elements[0] * Left.Elements[3] + Right.Elements[1] * Left.Elements[7] + Right.Elements[2] * Left.Elements[11] + Right.Elements[3] * Left.Elements[15];
		Result.Elements[4] = Right.Elements[4] * Left.Elements[0] + Right.Elements[5] * Left.Elements[4] + Right.Elements[6] * Left.Elements[8] + Right.Elements[7] * Left.Elements[12];
		Result.Elements[5] = Right.Elements[4] * Left.Elements[1] + Right.Elements[5] * Left.Elements[5] + Right.Elements[6] * Left.Elements[9] + Right.Elements[7] * Left.Elements[13];
		Result.Elements[6] = Right.Elements[4] * Left.Elements[2] + Right.Elements[5] * Left.Elements[6] + Right.Elements[6] * Left.Elements[10] + Right.Elements[7] * Left.Elements[14];
		Result.Elements[7] = Right.Elements[4] * Left.Elements[3] + Right.Elements[5] * Left.Elements[7] + Right.Elements[6] * Left.Elements[11] + Right.Elements[7] * Left.Elements[15];
		Result.Elements[8] = Right.Elements[8] * Left.Elements[0] + Right.Elements[9] * Left.Elements[4] + Right.Elements[10] * Left.Elements[8] + Right.Elements[11] * Left.Elements[12];
		Result.Elements[9] = Right.Elements[8] * Left.Elements[1] + Right.Elements[9] * Left.Elements[5] + Right.Elements[10] * Left.Elements[9] + Right.Elements[11] * Left.Elements[13];
		Result.Elements[10] = Right.Elements[8] * Left.Elements[2] + Right.Elements[9] * Left.Elements[6] + Right.Elements[10] * Left.Elements[10] + Right.Elements[11] * Left.Elements[14];
		Result.Elements[11] = Right.Elements[8] * Left.Elements[3] + Right.Elements[9] * Left.Elements[7] + Right.Elements[10] * Left.Elements[11] + Right.Elements[11] * Left.Elements[15];
		Result.Elements[12] = Right.Elements[12] * Left.Elements[0] + Right.Elements[13] * Left.Elements[4] + Right.Elements[14] * Left.Elements[8] + Right.Elements[15] * Left.Elements[12];
		Result.Elements[13] = Right.Elements[12] * Left.Elements[1] + Right.Elements[13] * Left.Elements[5] + Right.Elements[14] * Left.Elements[9] + Right.Elements[15] * Left.Elements[13];
		Result.Elements[14] = Right.Elements[12] * Left.Elements[2] + Right.Elements[13] * Left.Elements[6] + Right.Elements[14] * Left.Elements[10] + Right.Elements[15] * Left.Elements[14];
		Result.Elements[15] = Right.Elements[12] * Left.Elements[3] + Right.Elements[13] * Left.Elements[7] + Right.Elements[14] * Left.Elements[11] + Right.Elements[15] * Left.Elements[15];


		return Result;
	}

	Matrix4x4& operator*=(const Matrix4x4& Right)
	{
		*this = Multiply(*this, Right);

		return *this;
	}
};

struct VertexData
{
	Vector3 Position;
	Vector4 Color;
};

struct ConstantBufferData
{
	Matrix4x4 MVP;
};

void OutputShaderErrorMessage(ID3D10Blob* errorMessage)
{
	char* compileErrors;
	unsigned long long bufferSize, i;

	// Get a pointer to the error message text buffer.
	compileErrors = (char*)(errorMessage->GetBufferPointer());

	// Get the length of the message.
	bufferSize = errorMessage->GetBufferSize();

	char* Text = new char[bufferSize];
	char* ArrayPointer = Text;
	// Write out the error message.
	for (i = 0; i < bufferSize; i++, ArrayPointer++)
	{
		*ArrayPointer = compileErrors[i];
	}

	OutputDebugString(Text);

	delete Text;

	// Release the error message.
	errorMessage->Release();
	errorMessage = 0;

	return;
}

#define D3D_SAFE_RELEASE(x) \
if(x)\
	x->Release();\


int CALLBACK WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	WNDCLASS WindowClass = { };
	WindowClass.lpfnWndProc = WindowProc;
	WindowClass.hInstance = hInstance;
	WindowClass.lpszClassName = "Snake";

	if (RegisterClass(&WindowClass) == 0)
	{
		ShowSystemErrorMessage("Error Registering WNDCLASS");
		return 0;
	}

	HWND WindowHandle = CreateWindowEx(0, WindowClass.lpszClassName, "Snake", WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, CW_USEDEFAULT, 800, 600, 0, 0, hInstance, 0);

	if (WindowHandle == NULL)
	{
		ShowSystemErrorMessage("Error Calling CreateWindowEx");
		return 0;
	}

	ShowWindow(WindowHandle, SW_SHOW);

	D3D_FEATURE_LEVEL FeatureLevel;
	ID3D11Device* Device = nullptr;
	ID3D11DeviceContext* DeviceContext = nullptr;
	IDXGISwapChain* SwapChain = nullptr;
	ID3D11RenderTargetView* RenderTargetView;
	ID3D11DepthStencilView* DepthStencilView;
	ID3D11Texture2D* DepthStencilTexture;

	UINT D3D11DeviceFlag = 0;
#if defined(DEBUG) || defined(_DEBUG)
	D3D11DeviceFlag = D3D11_CREATE_DEVICE_DEBUG;
#endif 

	DXGI_SWAP_CHAIN_DESC SwapChainDesc;
	SwapChainDesc.BufferCount = 1;
	SwapChainDesc.BufferDesc.Width = 800;
	SwapChainDesc.BufferDesc.Height = 600;
	SwapChainDesc.BufferDesc.RefreshRate.Numerator = 60;
	SwapChainDesc.BufferDesc.RefreshRate.Denominator = 1;
	SwapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	SwapChainDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
	SwapChainDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	SwapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	SwapChainDesc.Flags = 0;
	SwapChainDesc.OutputWindow = WindowHandle;
	SwapChainDesc.SampleDesc.Count = 1;
	SwapChainDesc.SampleDesc.Quality = 0;
	SwapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
	SwapChainDesc.Windowed = true;

	if (FAILED(D3D11CreateDeviceAndSwapChain(0, D3D_DRIVER_TYPE_HARDWARE, 0, D3D11DeviceFlag, 0, 0, D3D11_SDK_VERSION, &SwapChainDesc, &SwapChain, &Device, &FeatureLevel, &DeviceContext)))
	{
		ShowSystemErrorMessage("Failed to create Direct3D 11 Context");
		return 0;
	}

	if (FeatureLevel < D3D_FEATURE_LEVEL_11_0)
	{
		D3D_SAFE_RELEASE(SwapChain);
		D3D_SAFE_RELEASE(Device);
		D3D_SAFE_RELEASE(DeviceContext);
		ShowSystemErrorMessage("Failed to create Direct3D 11 Context");
		return 0;
	}

	ID3D11Texture2D* BackBuffer = nullptr;
	SwapChain->GetBuffer(0, IID_PPV_ARGS(&BackBuffer));
	Device->CreateRenderTargetView(BackBuffer, 0, &RenderTargetView);
	BackBuffer->Release();

	D3D11_TEXTURE2D_DESC DepthStencilDesc;
	DepthStencilDesc.Width = 800;
	DepthStencilDesc.Height = 600;
	DepthStencilDesc.MipLevels = 1;
	DepthStencilDesc.ArraySize = 1;
	DepthStencilDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	DepthStencilDesc.SampleDesc.Count = 1;
	DepthStencilDesc.SampleDesc.Quality = 0;
	DepthStencilDesc.Usage = D3D11_USAGE_DEFAULT;
	DepthStencilDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	DepthStencilDesc.CPUAccessFlags = 0;
	DepthStencilDesc.MiscFlags = 0;

	HRESULT Result = Device->CreateTexture2D(&DepthStencilDesc, NULL, &DepthStencilTexture);
	if (FAILED(Result))
	{
		return false;
	}

	Result = Device->CreateDepthStencilView(DepthStencilTexture, 0, &DepthStencilView);
	if (FAILED(Result))
	{
		return false;
	}

	DeviceContext->OMSetRenderTargets(1, &RenderTargetView, DepthStencilView);

	D3D11_VIEWPORT Viewport;
	Viewport.TopLeftX = 0;
	Viewport.TopLeftY = 0;
	Viewport.Width = 800;
	Viewport.Height = 600;
	Viewport.MinDepth = 0.0f;
	Viewport.MaxDepth = 1.0f;

	DeviceContext->RSSetViewports(1, &Viewport);

	ID3D11VertexShader* VertexShader = nullptr;
	ID3D11PixelShader* PixelShader = nullptr;

	ID3D10Blob* VertexShaderBlob = nullptr;
	ID3D10Blob* PixelShaderBlob = nullptr;
	ID3D10Blob* ShaderErrorBlob = nullptr;
	Result = D3DCompile(ShaderCode, sizeof(ShaderCode), NULL, NULL, NULL, "VS", "vs_5_0", D3DCOMPILE_DEBUG, 0, &VertexShaderBlob, &ShaderErrorBlob);
	if (FAILED(Result))
	{
		OutputShaderErrorMessage(ShaderErrorBlob);
		ShowSystemErrorMessage("Failed to compile shader");
		return 0;
	}

	D3DCompile(ShaderCode, sizeof(ShaderCode), NULL, NULL, NULL, "PS", "ps_5_0", D3DCOMPILE_DEBUG, 0, &PixelShaderBlob, &ShaderErrorBlob);
	if (FAILED(Result))
	{
		OutputShaderErrorMessage(ShaderErrorBlob);
		ShowSystemErrorMessage("Failed to compile shader");
		return 0;
	}

	Device->CreateVertexShader(VertexShaderBlob->GetBufferPointer(), VertexShaderBlob->GetBufferSize(), 0, &VertexShader);
	Device->CreatePixelShader(PixelShaderBlob->GetBufferPointer(), PixelShaderBlob->GetBufferSize(), 0, &PixelShader);

	D3D11_INPUT_ELEMENT_DESC Elements[2];
	Elements[0].SemanticName = "POSITION";
	Elements[0].SemanticIndex = 0;
	Elements[0].AlignedByteOffset = 0;
	Elements[0].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	Elements[0].InputSlot = 0;
	Elements[0].InstanceDataStepRate = 0;
	Elements[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;

	Elements[1].SemanticName = "COLOR";
	Elements[1].SemanticIndex = 0;
	Elements[1].AlignedByteOffset = sizeof(float) * 3;
	Elements[1].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	Elements[1].InputSlot = 0;
	Elements[1].InstanceDataStepRate = 0;
	Elements[1].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;

	ID3D11InputLayout* InputLayout = nullptr;
	Device->CreateInputLayout(Elements, 2, VertexShaderBlob->GetBufferPointer(), VertexShaderBlob->GetBufferSize(), &InputLayout);

	D3D_SAFE_RELEASE(VertexShaderBlob);
	D3D_SAFE_RELEASE(PixelShaderBlob);
	D3D_SAFE_RELEASE(ShaderErrorBlob);

	ID3D11Buffer* ConstantBuffer = nullptr;

	D3D11_BUFFER_DESC ConstantBufferDesc = {  };
	ConstantBufferDesc.ByteWidth = sizeof(ConstantBufferData);
	ConstantBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	ConstantBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	ConstantBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	ConstantBufferDesc.MiscFlags = 0;
	ConstantBufferDesc.StructureByteStride = 0;

	Device->CreateBuffer(&ConstantBufferDesc, 0, &ConstantBuffer);

	ID3D11Buffer* VertexBuffer = nullptr;
	ID3D11Buffer* IndexBuffer = nullptr;

	D3D11_BUFFER_DESC VertexBufferDesc = {  };
	VertexBufferDesc.ByteWidth = sizeof(VertexData) * 4;
	VertexBufferDesc.Usage = D3D11_USAGE_IMMUTABLE;
	VertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	VertexBufferDesc.CPUAccessFlags = 0;
	VertexBufferDesc.MiscFlags = 0;
	VertexBufferDesc.StructureByteStride = 0;

	Vector4 RectangleColor = Vector4(1, 0, 0, 1);

	VertexData VBD[4];
	VBD[0].Position = Vector3(-0.5, 0.5, 0);
	VBD[0].Color = RectangleColor;

	VBD[1].Position = Vector3(0.5, -0.5, 0);
	VBD[1].Color = RectangleColor;

	VBD[2].Position = Vector3(-0.5, -0.5, 0);
	VBD[2].Color = RectangleColor;

	VBD[3].Position = Vector3(0.5, 0.5, 0);
	VBD[3].Color = RectangleColor;

	D3D11_SUBRESOURCE_DATA VertexBufferData;
	VertexBufferData.SysMemPitch = 0;
	VertexBufferData.SysMemSlicePitch = 0;
	VertexBufferData.pSysMem = VBD;

	Result = Device->CreateBuffer(&VertexBufferDesc, &VertexBufferData, &VertexBuffer);
	if (FAILED(Result))
	{
		ShowSystemErrorMessage("Failed to create VertexBuffer");
	}

	D3D11_BUFFER_DESC IndexBufferDesc = {  };
	IndexBufferDesc.ByteWidth = sizeof(unsigned int) * 6;
	IndexBufferDesc.Usage = D3D11_USAGE_IMMUTABLE;
	IndexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	IndexBufferDesc.CPUAccessFlags = 0;
	IndexBufferDesc.MiscFlags = 0;
	IndexBufferDesc.StructureByteStride = 0;

	unsigned int IBD[6];
	IBD[0] = 0;
	IBD[1] = 1;
	IBD[2] = 2;
	IBD[3] = 0;
	IBD[4] = 3;
	IBD[5] = 1;

	D3D11_SUBRESOURCE_DATA IndexBufferData;
	IndexBufferData.SysMemPitch = 0;
	IndexBufferData.SysMemSlicePitch = 0;
	IndexBufferData.pSysMem = IBD;

	Result = Device->CreateBuffer(&IndexBufferDesc, &IndexBufferData, &IndexBuffer);
	if (FAILED(Result))
	{
		ShowSystemErrorMessage("Failed to create IndexBuffer");
	}

	LARGE_INTEGER Frequency;
	LARGE_INTEGER LastCounter;
	QueryPerformanceFrequency(&Frequency);
	QueryPerformanceCounter(&LastCounter);
	float ElapsedTime = 0;

	MSG Message = {};
	while (bRun)
	{
		while (PeekMessage(&Message, WindowHandle, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&Message);
			DispatchMessage(&Message);
		}

		Vector4 Color = Vector4(0.0, 0.0, 1.0, 1.0);
		DeviceContext->ClearRenderTargetView(RenderTargetView, Color.Components);
		DeviceContext->ClearDepthStencilView(DepthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

		DeviceContext->VSSetShader(VertexShader, 0, 0);
		DeviceContext->PSSetShader(PixelShader, 0, 0);
		DeviceContext->IASetInputLayout(InputLayout);

		UINT Stride = sizeof(VertexData);
		UINT Offset = 0;
		DeviceContext->IASetVertexBuffers(0, 1, &VertexBuffer, &Stride, &Offset);
		DeviceContext->IASetIndexBuffer(IndexBuffer, DXGI_FORMAT_R32_UINT, 0);

		D3D11_MAPPED_SUBRESOURCE ConstantBufferSubresource;
		DeviceContext->Map(ConstantBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &ConstantBufferSubresource);
		ConstantBufferData* Data = (ConstantBufferData*)ConstantBufferSubresource.pData;
		Data->MVP = Matrix4x4::Identity();
		Data->MVP *= Matrix4x4::Ortographic(800, 600, 0, 100);
		Data->MVP *= Matrix4x4::Translation(0, 0, 0);
		Data->MVP *= Matrix4x4::Scaling(100,100, 0);

		DeviceContext->Unmap(ConstantBuffer, 0);

		DeviceContext->VSSetConstantBuffers(0, 1, &ConstantBuffer);

		DeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		DeviceContext->DrawIndexed(6, 0, 0);

		SwapChain->Present(0, 0);

		LARGE_INTEGER CurrentCounter;
		QueryPerformanceCounter(&CurrentCounter);

		LONGLONG ElapsedCounter = CurrentCounter.QuadPart - LastCounter.QuadPart;
		ElapsedTime = (float(ElapsedCounter) / float(Frequency.QuadPart));

		char Buffer[2048];
		snprintf(Buffer, sizeof(Buffer), "Snake | m/s: %s | fps: %s", std::to_string(ElapsedTime*1000.0f).c_str(), std::to_string((int)trunc(1 / (ElapsedTime))).c_str());
		SetWindowText(WindowHandle, Buffer);

		LastCounter = CurrentCounter;
	}

	D3D_SAFE_RELEASE(VertexBuffer);
	D3D_SAFE_RELEASE(IndexBuffer);
	D3D_SAFE_RELEASE(ConstantBuffer);
	D3D_SAFE_RELEASE(InputLayout);
	D3D_SAFE_RELEASE(VertexShader);
	D3D_SAFE_RELEASE(PixelShader);
	D3D_SAFE_RELEASE(DepthStencilView);
	D3D_SAFE_RELEASE(DepthStencilTexture);
	D3D_SAFE_RELEASE(RenderTargetView);
	D3D_SAFE_RELEASE(SwapChain);
	D3D_SAFE_RELEASE(Device);
	D3D_SAFE_RELEASE(DeviceContext);

	return 0;
}

void ShowSystemErrorMessage(const char* Message)
{
	MessageBox(NULL, Message, "Error", MB_OK | MB_ICONERROR);
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	if (uMsg == WM_CLOSE || uMsg == WM_DESTROY)
	{
		bRun = false;
		return 0;
	}
	else
	{
		return DefWindowProc(hwnd, uMsg, wParam, lParam);

	}
}