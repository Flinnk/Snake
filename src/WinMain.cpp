#include <Windows.h>
#include <d3d11.h>

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
void ShowSystemErrorMessage(const char* Message);

static bool bRun = true;

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
		if (Device)
			Device->Release();
		if (DeviceContext)
			Device->Release();
		if (SwapChain)
			SwapChain->Release();
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

	MSG Message = {};
	while (bRun)
	{
		while (PeekMessage(&Message, WindowHandle, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&Message);
			DispatchMessage(&Message);
		}

		float Color[] = { 0.0,0.0,1.0,1.0, };
		DeviceContext->ClearRenderTargetView(RenderTargetView, Color);
		DeviceContext->ClearDepthStencilView(DepthStencilView,D3D11_CLEAR_DEPTH|D3D11_CLEAR_STENCIL,1.0f,0);

		SwapChain->Present(1, 0);
	}

	if (DepthStencilView)
		DepthStencilView->Release();
	if (DepthStencilTexture)
		DepthStencilTexture->Release();
	if (RenderTargetView)
		RenderTargetView->Release();
	if (SwapChain)
		SwapChain->Release();
	if (Device)
		Device->Release();
	if (DeviceContext)
		DeviceContext->Release();

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