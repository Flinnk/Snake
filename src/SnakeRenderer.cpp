#include <SnakeRenderer.h>
#include <d3dcompiler.h>
#include <stb/stb_image.h>



static const char ShaderCode[] = "\
	cbuffer BufferPerBatch  \
	{	float4x4 Projection;\
	}\
\
	struct VertexInput\
{\
	float4 Position : POSITION;\
	float4 Color : COLOR;\
	float2 UV: TEXCOORD0;\
};\
\
struct PixelInput\
{\
	float4 Position : SV_POSITION;\
	float4 Color : COLOR;\
	float2 UV: TEXCOORD0;\
};\
\
PixelInput VS(VertexInput Input)\
{\
	PixelInput Output;\
	Input.Position.w = 1;\
	Output.Color = Input.Color;\
	Output.Position = mul(Projection,Input.Position);\
	Output.UV = Input.UV;\
	return Output;\
}\
\
\ Texture2D shaderTexture;\
\ SamplerState sampleType;\
\
float4 PS(PixelInput Input) : SV_TARGET\
{\
	return shaderTexture.Sample(sampleType,Input.UV)*Input.Color;\
}\
";


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


bool Renderer::Initialize(HWND WindowHandle, int InWidth, int InHeight)
{
	Width = InWidth;
	Height = InHeight;

	UINT D3D11DeviceFlag = 0;
#if defined(DEBUG) || defined(_DEBUG)
	D3D11DeviceFlag = D3D11_CREATE_DEVICE_DEBUG;
#endif 

	DXGI_SWAP_CHAIN_DESC SwapChainDesc;
	SwapChainDesc.BufferCount = 1;
	SwapChainDesc.BufferDesc.Width = Width;
	SwapChainDesc.BufferDesc.Height = Height;
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
		return false;
	}

	if (FeatureLevel < D3D_FEATURE_LEVEL_11_0)
	{
		return false;
	}

	IDXGIDevice * pDXGIDevice;
	Device->QueryInterface(__uuidof(IDXGIDevice), (void **)&pDXGIDevice);

	IDXGIAdapter * pDXGIAdapter;
	pDXGIDevice->GetParent(__uuidof(IDXGIAdapter), (void **)&pDXGIAdapter);

	IDXGIFactory * pIDXGIFactory;
	pDXGIAdapter->GetParent(__uuidof(IDXGIFactory), (void **)&pIDXGIFactory);

	pIDXGIFactory->MakeWindowAssociation(WindowHandle, DXGI_MWA_NO_ALT_ENTER);//Disable ALT+ENTER to go FULLSCREEN

	pIDXGIFactory->Release();
	pDXGIAdapter->Release();
	pDXGIDevice->Release();

	ID3D11Texture2D* BackBuffer = nullptr;
	SwapChain->GetBuffer(0, IID_PPV_ARGS(&BackBuffer));
	Device->CreateRenderTargetView(BackBuffer, 0, &RenderTargetView);
	BackBuffer->Release();

	D3D11_TEXTURE2D_DESC DepthStencilDesc;
	DepthStencilDesc.Width = Width;
	DepthStencilDesc.Height = Height;
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

	DeviceContext->OMSetRenderTargets(1, &RenderTargetView, NULL/*DepthStencilView*/);//No necesitamos depth testing

	D3D11_VIEWPORT Viewport;
	Viewport.TopLeftX = 0;
	Viewport.TopLeftY = 0;
	Viewport.Width = Width;
	Viewport.Height = Height;
	Viewport.MinDepth = 0.0f;
	Viewport.MaxDepth = 1.0f;

	DeviceContext->RSSetViewports(1, &Viewport);

	ID3D10Blob* VertexShaderBlob = nullptr;
	ID3D10Blob* PixelShaderBlob = nullptr;
	ID3D10Blob* ShaderErrorBlob = nullptr;

	Result = D3DCompile(ShaderCode, sizeof(ShaderCode), NULL, NULL, NULL, "VS", "vs_5_0", D3DCOMPILE_DEBUG, 0, &VertexShaderBlob, &ShaderErrorBlob);
	if (FAILED(Result))
	{
		OutputShaderErrorMessage(ShaderErrorBlob);
		return false;
	}

	D3DCompile(ShaderCode, sizeof(ShaderCode), NULL, NULL, NULL, "PS", "ps_5_0", D3DCOMPILE_DEBUG, 0, &PixelShaderBlob, &ShaderErrorBlob);
	if (FAILED(Result))
	{
		OutputShaderErrorMessage(ShaderErrorBlob);
		return false;
	}

	Result = Device->CreateVertexShader(VertexShaderBlob->GetBufferPointer(), VertexShaderBlob->GetBufferSize(), 0, &VertexShader);
	if (FAILED(Result))
	{
		return false;
	}

	Result = Device->CreatePixelShader(PixelShaderBlob->GetBufferPointer(), PixelShaderBlob->GetBufferSize(), 0, &PixelShader);
	if (FAILED(Result))
	{
		return false;
	}

	D3D11_INPUT_ELEMENT_DESC Elements[3];
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

	Elements[2].SemanticName = "TEXCOORD";
	Elements[2].SemanticIndex = 0;
	Elements[2].AlignedByteOffset = Elements[1].AlignedByteOffset + sizeof(float) * 4;
	Elements[2].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	Elements[2].InputSlot = 0;
	Elements[2].InstanceDataStepRate = 0;
	Elements[2].Format = DXGI_FORMAT_R32G32_FLOAT;

	Result = Device->CreateInputLayout(Elements, 3, VertexShaderBlob->GetBufferPointer(), VertexShaderBlob->GetBufferSize(), &InputLayout);
	if (FAILED(Result))
	{
		return false;
	}

	D3D_SAFE_RELEASE(VertexShaderBlob);
	D3D_SAFE_RELEASE(PixelShaderBlob);
	D3D_SAFE_RELEASE(ShaderErrorBlob);


	D3D11_BUFFER_DESC ConstantBufferDesc = {  };
	ConstantBufferDesc.ByteWidth = sizeof(ConstantBufferData);
	ConstantBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	ConstantBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	ConstantBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	ConstantBufferDesc.MiscFlags = 0;
	ConstantBufferDesc.StructureByteStride = 0;

	Result = Device->CreateBuffer(&ConstantBufferDesc, 0, &ConstantBuffer);
	if (FAILED(Result))
	{
		return false;
	}

	D3D11_BUFFER_DESC VertexBufferDesc = {  };
	VertexBufferDesc.ByteWidth = sizeof(VertexData) * NUM_VERTICES;
	VertexBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	VertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	VertexBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	VertexBufferDesc.MiscFlags = 0;
	VertexBufferDesc.StructureByteStride = 0;

	/*Vector4 RectangleColor = Vector4(1, 0, 0, 1);

	VertexData VBD[4];
	VBD[0].Position = Vector3(0, 1, 0);
	VBD[0].Color = RectangleColor;

	VBD[1].Position = Vector3(1, 0, 0);
	VBD[1].Color = RectangleColor;

	VBD[2].Position = Vector3(0, 0, 0);
	VBD[2].Color = RectangleColor;

	VBD[3].Position = Vector3(1, 1, 0);
	VBD[3].Color = RectangleColor;*/

	Result = Device->CreateBuffer(&VertexBufferDesc, 0, &VertexBuffer);
	if (FAILED(Result))
	{
		return false;
	}

	D3D11_BUFFER_DESC IndexBufferDesc = {  };
	IndexBufferDesc.ByteWidth = sizeof(unsigned int) * NUM_INDICES;
	IndexBufferDesc.Usage = D3D11_USAGE_IMMUTABLE;
	IndexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	IndexBufferDesc.CPUAccessFlags = 0;
	IndexBufferDesc.MiscFlags = 0;
	IndexBufferDesc.StructureByteStride = 0;

	unsigned int IBD[NUM_INDICES];
	unsigned int LastIndex = 0;
	for (int Index = 0; Index < NUM_INDICES; Index += 6)
	{
		IBD[Index] = LastIndex;
		IBD[Index + 1] = LastIndex + 1;
		IBD[Index + 2] = LastIndex + 2;
		IBD[Index + 3] = LastIndex + 3;
		IBD[Index + 4] = LastIndex + 1;
		IBD[Index + 5] = LastIndex;
		LastIndex += 4;
	}

	/*IBD[0] = 0;
	IBD[1] = 1;
	IBD[2] = 2;
	IBD[3] = 3;
	IBD[4] = 1;
	IBD[5] = 0;*/

	D3D11_SUBRESOURCE_DATA IndexBufferData;
	IndexBufferData.SysMemPitch = 0;
	IndexBufferData.SysMemSlicePitch = 0;
	IndexBufferData.pSysMem = IBD;

	Result = Device->CreateBuffer(&IndexBufferDesc, &IndexBufferData, &IndexBuffer);
	if (FAILED(Result))
	{
		return false;
	}

	//Set drawing state

	DeviceContext->VSSetShader(VertexShader, 0, 0);
	DeviceContext->PSSetShader(PixelShader, 0, 0);
	DeviceContext->IASetInputLayout(InputLayout);

	UINT Stride = sizeof(VertexData);
	UINT Offset = 0;
	DeviceContext->IASetVertexBuffers(0, 1, &VertexBuffer, &Stride, &Offset);
	DeviceContext->IASetIndexBuffer(IndexBuffer, DXGI_FORMAT_R32_UINT, 0);

	DeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	D3D11_MAPPED_SUBRESOURCE ConstantBufferSubresource;
	DeviceContext->Map(ConstantBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &ConstantBufferSubresource);
	ConstantBufferData* Data = (ConstantBufferData*)ConstantBufferSubresource.pData;
	Data->MVP = Matrix4x4::Identity();
	Data->MVP *= Matrix4x4::Ortographic(0, Width, 0, Height, 0, 100);//Intercambiar ViewTop/ViewBottom hace que pasemos el cordenadas de con origen arriba izquierda a abajo izquierda

	DeviceContext->Unmap(ConstantBuffer, 0);
	DeviceContext->VSSetConstantBuffers(0, 1, &ConstantBuffer);

	//Create 1x1 pixel white texture
	int Channels = 4;
	unsigned char ImageData[4] = { 255,255,255,255 };

	// Create texture
	D3D11_TEXTURE2D_DESC desc;
	desc.Width = 1;
	desc.Height = 1;
	desc.MipLevels = 1;
	desc.ArraySize = 1;
	desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	desc.SampleDesc.Count = 1;
	desc.SampleDesc.Quality = 0;
	desc.Usage = D3D11_USAGE_DEFAULT;
	desc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
	desc.CPUAccessFlags = 0;
	desc.MiscFlags = 0;

	D3D11_SUBRESOURCE_DATA initData;
	initData.pSysMem = ImageData;
	initData.SysMemPitch = Width * Channels * sizeof(char);
	initData.SysMemSlicePitch = 0;

	ID3D11Texture2D* STexture = nullptr;
	ID3D11ShaderResourceView* TextureView = nullptr;
	Device->CreateTexture2D(&desc, &initData, &STexture);
	if (STexture)
	{
		D3D11_SHADER_RESOURCE_VIEW_DESC SRVDesc;
		memset(&SRVDesc, 0, sizeof(SRVDesc));
		SRVDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		SRVDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		SRVDesc.Texture2D.MipLevels = 1;

		HRESULT hr = Device->CreateShaderResourceView(STexture, &SRVDesc, &TextureView);
		if (FAILED(hr))
		{
			STexture->Release();
			return false;
		}
	}

	D3D11_SAMPLER_DESC SamplerDesc;
	SamplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	SamplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	SamplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	SamplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	SamplerDesc.MipLODBias = 0.0f;
	SamplerDesc.MaxAnisotropy = 1;
	SamplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	SamplerDesc.BorderColor[0] = 0;
	SamplerDesc.BorderColor[1] = 0;
	SamplerDesc.BorderColor[2] = 0;
	SamplerDesc.BorderColor[3] = 0;
	SamplerDesc.MinLOD = 0;
	SamplerDesc.MaxLOD = D3D11_FLOAT32_MAX;
	Result = Device->CreateSamplerState(&SamplerDesc, &SamplerState);
	if (FAILED(Result))
	{
		return false;
	}

	SpriteTexture = Texture(STexture, TextureView);
	DeviceContext->PSSetSamplers(0, 1, &SamplerState);

	return true;
}

void Renderer::Clear(const Vector4& Color)
{
	DeviceContext->ClearRenderTargetView(RenderTargetView, Color.Components);
	DeviceContext->ClearDepthStencilView(DepthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
}

void Renderer::Present()
{
	SwapChain->Present(0, 0);
}

void Renderer::Begin()
{
	IndicesToDraw = 0;
	D3D11_MAPPED_SUBRESOURCE BufferSubresource;
	DeviceContext->Map(VertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &BufferSubresource);
	VertexBufferPointer = (VertexData*)BufferSubresource.pData;
}

void Renderer::End()
{
	DeviceContext->Unmap(VertexBuffer, 0);
	DeviceContext->PSSetShaderResources(0, 1, &LastDrawnTexture->TextureView);
	DeviceContext->DrawIndexed(IndicesToDraw, 0, 0);
}

void Renderer::DrawSprite(Vector3 Position, Vector3 Size, Vector3 Offset, Vector4 Color, Texture* Texture)
{
	if (VertexBufferPointer == nullptr)
		return;

	if (Texture == nullptr)
		Texture = &SpriteTexture;

	if (LastDrawnTexture && LastDrawnTexture != Texture)
	{
		End();
		Begin();
	}

	LastDrawnTexture = Texture;

	//Vertex1
	VertexBufferPointer->Position = Vector3(Position.X + Offset.X, Position.Y + Size.Y - Offset.Y, Position.Z);
	VertexBufferPointer->Color = Color;
	VertexBufferPointer->UV = Vector3(0, 0, 0);
	++VertexBufferPointer;

	//Vertex2
	VertexBufferPointer->Position = Vector3(Position.X + Size.X - Offset.X, Position.Y + Offset.Y, Position.Z);
	VertexBufferPointer->Color = Color;
	VertexBufferPointer->UV = Vector3(1, 1, 0);

	++VertexBufferPointer;

	//Vertex3
	VertexBufferPointer->Position = Vector3(Position.X + Offset.X, Position.Y + Offset.Y, Position.Z);
	VertexBufferPointer->Color = Color;
	VertexBufferPointer->UV = Vector3(0, 1, 0);

	++VertexBufferPointer;

	//Vertex4
	VertexBufferPointer->Position = Vector3(Position.X + Size.X - Offset.Y, Position.Y + Size.Y - Offset.Y, Position.Z);
	VertexBufferPointer->Color = Color;
	VertexBufferPointer->UV = Vector3(1, 0, 0);

	++VertexBufferPointer;

	IndicesToDraw += 6;

	if (IndicesToDraw >= NUM_INDICES)
	{
		End();
		Begin();
	}

}

Texture Renderer::LoadTextureFromFile(const char* Path)
{
	int Width = 0;
	int Height = 0;
	int Channels = 0;
	stbi_set_flip_vertically_on_load(false);
	unsigned char* ImageData = stbi_load(Path, &Width, &Height, &Channels, 0);

	// Create texture
	D3D11_TEXTURE2D_DESC desc;
	desc.Width = Width;
	desc.Height = Height;
	desc.MipLevels = 1;
	desc.ArraySize = 1;
	desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	desc.SampleDesc.Count = 1;
	desc.SampleDesc.Quality = 0;
	desc.Usage = D3D11_USAGE_DEFAULT;
	desc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
	desc.CPUAccessFlags = 0;
	desc.MiscFlags = 0;

	D3D11_SUBRESOURCE_DATA initData;
	initData.pSysMem = ImageData;
	initData.SysMemPitch = Width * Channels * sizeof(char);
	initData.SysMemSlicePitch = 0;

	ID3D11Texture2D* STexture = nullptr;
	ID3D11ShaderResourceView* TextureView = nullptr;
	Device->CreateTexture2D(&desc, &initData, &STexture);
	if (STexture)
	{
		D3D11_SHADER_RESOURCE_VIEW_DESC SRVDesc;
		memset(&SRVDesc, 0, sizeof(SRVDesc));
		SRVDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		SRVDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		SRVDesc.Texture2D.MipLevels = 1;

		HRESULT hr = Device->CreateShaderResourceView(STexture, &SRVDesc, &TextureView);
		if (FAILED(hr))
		{
			STexture->Release();
		}
	}

	stbi_image_free(ImageData);

	return Texture(STexture, TextureView);
}

Texture Renderer::LoadTextureFromMemory(const unsigned char* Data, int TextureWidth, int TextureHeight, int TextureChannels)
{
	// Create texture
	D3D11_TEXTURE2D_DESC desc;
	desc.Width = TextureWidth;
	desc.Height = TextureHeight;
	desc.MipLevels = 1;
	desc.ArraySize = 1;
	desc.Format = DXGI_FORMAT_R8_UNORM;
	desc.SampleDesc.Count = 1;
	desc.SampleDesc.Quality = 0;
	desc.Usage = D3D11_USAGE_DEFAULT;
	desc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
	desc.CPUAccessFlags = 0;
	desc.MiscFlags = 0;

	D3D11_SUBRESOURCE_DATA initData;
	initData.pSysMem = Data;
	initData.SysMemPitch = TextureWidth * TextureChannels * sizeof(char);
	initData.SysMemSlicePitch = 0;

	ID3D11Texture2D* STexture = nullptr;
	ID3D11ShaderResourceView* TextureView = nullptr;
	Device->CreateTexture2D(&desc, &initData, &STexture);
	if (STexture)
	{
		D3D11_SHADER_RESOURCE_VIEW_DESC SRVDesc;
		memset(&SRVDesc, 0, sizeof(SRVDesc));
		SRVDesc.Format = DXGI_FORMAT_R8_UNORM;
		SRVDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		SRVDesc.Texture2D.MipLevels = 1;

		HRESULT hr = Device->CreateShaderResourceView(STexture, &SRVDesc, &TextureView);
		if (FAILED(hr))
		{
			STexture->Release();
		}
	}

	return Texture(STexture, TextureView);
}

void Renderer::Release()
{
	D3D_SAFE_RELEASE(SamplerState);
	SpriteTexture.Release();
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
}