#ifndef DEVICEMANAGER_H
#define DEVICEMANAGER_H

#include "DebugHelper.h"
#include <DirectXMath.h>
#include <iostream>

#include "VertexTypes.h"
#include "Datatypes.h"
#include <vector>

using namespace DirectX;
using namespace std;

class DeviceManager{

public:

	DeviceManager();
	~DeviceManager();

	void Initialize(HWND windowHandle);
	void Shutdown();

	HRESULT CreateDeviceResources();
	HRESULT CreateWindowResources(HWND windowHandle);

	ID3D11Buffer* CreateStaticVertexBuffer(void* vertexData, unsigned vertexCount);
	ID3D11Buffer* CreateDynamicVertexBuffer(Vertex* vertexData, unsigned vertexCount);

	ID3D11Buffer* CreateIndexBuffer(unsigned int* indexData, int indexCount);

	HRESULT ConfigureBackBuffer();
	HRESULT ReleaseBackBuffer();
	HRESULT SetFullscreenMode();
	HRESULT SetWindowedMode();

	ID3D11Device*           GetDevice() { return gDevice; };
	ID3D11DeviceContext*    GetDeviceContext() { return gDeviceContext; };
	ID3D11RenderTargetView* const* GetRenderTarget() const { return &gBackbufferRTV; };
	ID3D11RenderTargetView* GetRTV() const { return gBackbufferRTV; };
	IDXGISwapChain* GetSwapChain() { return gSwapChain; };

	void Present();

private:

	vector<ID3D11Buffer*>vertexBuffers;

	//----------------------------------------------------------------------------------------------------------------------------------//
	// DirectX Devices
	//----------------------------------------------------------------------------------------------------------------------------------//
	ID3D11Device* gDevice;
	ID3D11DeviceContext* gDeviceContext;
	IDXGISwapChain* gSwapChain;

	//----------------------------------------------------------------------------------------------------------------------------------//
	// DXGI swap chain device resources
	//----------------------------------------------------------------------------------------------------------------------------------//
	ID3D11Texture2D* gBackBuffer;
	ID3D11RenderTargetView* gBackbufferRTV;

	//----------------------------------------------------------------------------------------------------------------------------------//
	// Metadata and description
	//----------------------------------------------------------------------------------------------------------------------------------//
	D3D_FEATURE_LEVEL gFeatureLevel;

	//----------------------------------------------------------------------------------------------------------------------------------//
	// Buffer description
	//----------------------------------------------------------------------------------------------------------------------------------//
	D3D11_BUFFER_DESC bufferDesc;
	D3D11_SUBRESOURCE_DATA subData;

};


#endif
