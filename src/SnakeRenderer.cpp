#include <SnakeRenderer.h>
#include <d3dcompiler.h>

#define D3D_SAFE_RELEASE(x) \
if(x)\
	x->Release();\

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
	Output.Position = mul(Projection,Input.Position);\
	return Output;\
}\
\
float4 PS(PixelInput Input) : SV_TARGET\
{\
	return Input.Color;\
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

	DeviceContext->OMSetRenderTargets(1, &RenderTargetView, DepthStencilView);

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

	Result = Device->CreateInputLayout(Elements, 2, VertexShaderBlob->GetBufferPointer(), VertexShaderBlob->GetBufferSize(), &InputLayout);
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
		LastIndex += Index + 4;
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
	DeviceContext->DrawIndexed(IndicesToDraw, 0, 0);
}

void Renderer::DrawRectangle(Vector3 Position, Vector3 Size, Vector4 Color)
{
	if (VertexBufferPointer == nullptr)
		return;

	//Vertex1
	VertexBufferPointer->Position = Vector3(Position.X, Position.Y+Size.Y, Position.Z);
	VertexBufferPointer->Color = Color;

	++VertexBufferPointer;

	//Vertex2
	VertexBufferPointer->Position = Vector3(Position.X+Size.X, Position.Y, Position.Z);
	VertexBufferPointer->Color = Color;

	++VertexBufferPointer;

	//Vertex3
	VertexBufferPointer->Position = Vector3(Position.X, Position.Y, Position.Z);
	VertexBufferPointer->Color = Color;

	++VertexBufferPointer;

	//Vertex4
	VertexBufferPointer->Position = Vector3(Position.X+Size.X, Position.Y+Size.Y, Position.Z);
	VertexBufferPointer->Color = Color;

	++VertexBufferPointer;

	IndicesToDraw += 6;
}

void Renderer::Release()
{
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