
#include "DeviceManager.h"

#include "ApplicationCore.h"

DeviceManager::DeviceManager() {

	
}

DeviceManager::~DeviceManager() {


}

void DeviceManager::Initialize(HWND windowHandle) {

	HRESULT hr = S_OK;

	hr = CreateDeviceResources();

	if (FAILED(hr)) {

		MessageBox(
			NULL,
			L"CRITICAL ERROR: Device resources could not be initialized\nClosing application...",
			L"ERROR",
			MB_OK);

		PostQuitMessage(0);
	}

	hr = CreateWindowResources(windowHandle);

	if (FAILED(hr)) {

		MessageBox(
			NULL,
			L"CRITICAL ERROR: Window resources could not be initialized\nClosing application...",
			L"ERROR",
			MB_OK);

		PostQuitMessage(0);
	}

	hr = ConfigureBackBuffer();

	if (FAILED(hr)) {

		MessageBox(
			NULL,
			L"CRITICAL ERROR: Back buffer could not be configured\nClosing application...",
			L"ERROR",
			MB_OK);

		PostQuitMessage(0);
	}
}

void DeviceManager::Shutdown() {

	SAFE_RELEASE(gDeviceContext);
	SAFE_RELEASE(gSwapChain);

	SAFE_RELEASE(gBackBuffer);
	SAFE_RELEASE(gBackbufferRTV);

//#ifdef _DEBUG
//	reportLiveObjects(gDevice);
//#endif

	SAFE_RELEASE(gDevice);

}

HRESULT DeviceManager::CreateDeviceResources() {

	HRESULT hr = S_OK;

	D3D_FEATURE_LEVEL levels[] = {

		D3D_FEATURE_LEVEL_11_0,
		D3D_FEATURE_LEVEL_11_1
	};

	UINT deviceFlags = D3D11_CREATE_DEVICE_BGRA_SUPPORT;

#if defined(DEBUG) || defined(_DEBUG)

	deviceFlags |= D3D11_CREATE_DEVICE_DEBUG;

#endif

	hr = D3D11CreateDevice(

		nullptr,
		D3D_DRIVER_TYPE_HARDWARE,
		0,
		deviceFlags,
		levels,
		ARRAYSIZE(levels),
		D3D11_SDK_VERSION,
		&gDevice,
		&gFeatureLevel,
		&gDeviceContext

	);

	if (FAILED(hr)) {

		cout << "Device Creation Error: Device, DeviceContext and Swap Chain could not be created" << endl;
		return S_FALSE;
	}

	return hr;

}

HRESULT DeviceManager::CreateWindowResources(HWND windowHandle) {

	HRESULT hr = S_OK;

	float w = ApplicationCore::GetInstance()->GetWindow()->GetWindowWidth();
	float h = ApplicationCore::GetInstance()->GetWindow()->GetWindowHeight();

	DXGI_SWAP_CHAIN_DESC swapChainDesc;
	ZeroMemory(&swapChainDesc, sizeof(DXGI_SWAP_CHAIN_DESC));
	swapChainDesc.Windowed = TRUE;
	swapChainDesc.BufferCount = 2;
	swapChainDesc.BufferDesc.Width = w;
	swapChainDesc.BufferDesc.Height = h;
	swapChainDesc.BufferDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapChainDesc.SampleDesc.Count = 1;
	swapChainDesc.SampleDesc.Quality = 0;
	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL;
	swapChainDesc.OutputWindow = windowHandle;

	IDXGIDevice* dxgiDevice = 0;
	hr = gDevice->QueryInterface(__uuidof(IDXGIDevice), (void **)& dxgiDevice);
	if (SUCCEEDED(hr)) {
	
	}
	
	IDXGIAdapter* dxgiAdapter = 0;
	hr = dxgiDevice->GetParent(__uuidof(IDXGIAdapter), (void **)& dxgiAdapter);
	if (SUCCEEDED(hr)) {

	}

	IDXGIFactory * dxgiFactory = 0;
	hr = dxgiAdapter->GetParent(__uuidof(IDXGIFactory), (void **)& dxgiFactory);
	if (SUCCEEDED(hr)) {

		dxgiFactory->CreateSwapChain(gDevice, &swapChainDesc, &gSwapChain);
	}
	
	dxgiFactory->Release();
	dxgiAdapter->Release();
	dxgiDevice->Release();

	return hr;
}

ID3D11Buffer* DeviceManager::CreateStaticVertexBuffer(void* vertexData, unsigned vertexCount) {

	HRESULT hr = S_OK;

	ZeroMemory(&bufferDesc, sizeof(bufferDesc));
	bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bufferDesc.ByteWidth = sizeof(Vertex) * vertexCount;
	bufferDesc.CPUAccessFlags = 0;
	bufferDesc.MiscFlags = 0;
	bufferDesc.StructureByteStride = 0;
	bufferDesc.Usage = D3D11_USAGE_DEFAULT;

	ZeroMemory(&subData, sizeof(subData));
	subData.pSysMem = vertexData;
	subData.SysMemPitch = 0;
	subData.SysMemSlicePitch = 0;

	ID3D11Buffer* buffer = nullptr;

	hr = gDevice->CreateBuffer(&bufferDesc, &subData, &buffer);

	if (FAILED(hr)) {

		MessageBox(
			NULL,
			L"CRITICAL ERROR: Vertex buffer could not be created\nClosing application...",
			L"ERROR",
			MB_OK);

		PostQuitMessage(0);
	}

	return buffer;
}

ID3D11Buffer* DeviceManager::CreateDynamicVertexBuffer(Vertex* vertexData, unsigned vertexCount) {

	HRESULT hr = S_OK;

	ZeroMemory(&bufferDesc, sizeof(bufferDesc));
	bufferDesc.ByteWidth = sizeof(Vertex) * vertexCount;
	bufferDesc.Usage = D3D11_USAGE_DYNAMIC;					// Set usage to be dynamic
	bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;		// We are required to write new data every frame to this buffer
	bufferDesc.MiscFlags = 0;
	bufferDesc.StructureByteStride = 0;

	ZeroMemory(&subData, sizeof(subData));
	subData.pSysMem = &vertexData[0];
	subData.SysMemPitch = 0;
	subData.SysMemSlicePitch = 0;

	ID3D11Buffer* buffer = nullptr;

	hr = gDevice->CreateBuffer(&bufferDesc, &subData, &buffer);

	if (FAILED(hr)) {

		MessageBox(
			NULL,
			L"CRITICAL ERROR: Vertex buffer could not be created\nClosing application...",
			L"ERROR",
			MB_OK);

		PostQuitMessage(0);
	}

	return buffer;
}

ID3D11Buffer* DeviceManager::CreateIndexBuffer(unsigned int* indexData, int indexCount) {

	HRESULT hr;

	// Create the buffer description
	ZeroMemory(&bufferDesc, sizeof(bufferDesc));
	bufferDesc.Usage = D3D11_USAGE_DEFAULT;
	bufferDesc.ByteWidth = sizeof(unsigned int) * indexCount;
	bufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	bufferDesc.CPUAccessFlags = 0;
	bufferDesc.MiscFlags = 0;

	// Set the subresource data

	D3D11_SUBRESOURCE_DATA initData;
	initData.pSysMem = indexData;
	initData.SysMemPitch = 0;
	initData.SysMemSlicePitch = 0;

	// Create the buffer
	ID3D11Buffer* buffer = nullptr;

	hr = gDevice->CreateBuffer(&bufferDesc, &initData, &buffer);

	if (FAILED(hr)) {

		MessageBox(
			NULL,
			L"CRITICAL ERROR: Index buffer could not be created\nClosing application...",
			L"ERROR",
			MB_OK);

		PostQuitMessage(0);
	}

	setDebugName(buffer, "STANDARD_MESH_INDEX_BUFFER");

	return buffer;
}

HRESULT DeviceManager::ConfigureBackBuffer() {

	HRESULT hr = S_OK;

	hr = gSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&gBackBuffer);

	setDebugName(gBackBuffer, "STANDARD_BACK_BUFFER_TEXTURE2D");

	if (FAILED(hr)) {

		// This error will arise when the back buffer could not be retrieved
		cout << "Buffer Error: Back buffer could not be retrieved" << endl;
		return S_FALSE;
	}

	hr = gDevice->CreateRenderTargetView(gBackBuffer, nullptr, &gBackbufferRTV);

	if (FAILED(hr)) {

		// This error is given because the render target view creation failed
		cout << "Render Target View Error: Render target view could not be created" << endl;
		return S_FALSE;
	}

	setDebugName(gBackbufferRTV, "STANDARD_BACK_BUFFER_RTV");

	return hr;
}

HRESULT DeviceManager::ReleaseBackBuffer() {

	HRESULT hr = S_OK;

	SAFE_RELEASE(gBackbufferRTV);
	SAFE_RELEASE(gBackBuffer);

	gDeviceContext->Flush();

	return hr;
}

HRESULT DeviceManager::SetFullscreenMode() {

	HRESULT hr = S_OK;

	hr = gSwapChain->SetFullscreenState(TRUE, NULL);

	ReleaseBackBuffer();

	hr = gSwapChain->ResizeBuffers(
		0,
		0, 0,
		DXGI_FORMAT_UNKNOWN,
		0
	);

	hr = ConfigureBackBuffer();

	return hr;
}

HRESULT DeviceManager::SetWindowedMode() {

	HRESULT hr = S_OK;

	hr = gSwapChain->SetFullscreenState(FALSE, NULL);

	ReleaseBackBuffer();

	hr = gSwapChain->ResizeBuffers(
		0,
		0, 0,
		DXGI_FORMAT_UNKNOWN,
		0
	);

	hr = ConfigureBackBuffer();

	return hr;
}


void DeviceManager::Present() {

	gSwapChain->Present(1, 0);
}