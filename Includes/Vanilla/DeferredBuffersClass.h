#ifndef DEFERREDBUFFERCLASS_H
#define DEFERREDBUFFERCLASS_H

#include <d3d11.h>
#include <d3dcompiler.h>
#include <DirectXMath.h>
#include <iostream>

#include <vector>
#include "VertexTypes.h"

#include <fstream>
#include <iostream>
#include <sstream>

#include "MacroDefinitions.h"

#include "DebugHelper.h"

#define BUFFER_COUNT 3

using namespace std;
using namespace DirectX;

class DeferredBuffersClass {

private:

	

public:

	DeferredBuffersClass();
	~DeferredBuffersClass();
	void Shutdown();

	bool Initialize(ID3D11Device* gDevice);
	bool SetupResources(ID3D11Device* gDevice);
	bool CreateDepthStencil(ID3D11Device* gDevice);
	void CreateViewport();

	void SetRenderTargets(ID3D11DeviceContext* gDeviceContext);
	void SetRenderTarget(ID3D11DeviceContext* gDeviceContext, int GBufferIndex);
	void SetViewPort(ID3D11DeviceContext* gDeviceContext);
	void ClearRenderTargets(ID3D11DeviceContext* gDeviceContext, float red, float green, float blue, float alpha);

	void SetStaticObjectBuffer(ID3D11DeviceContext* gDeviceContext, ID3D11Buffer* vertexBuffer);

	bool CreateRasterizerState(ID3D11Device* gDevice, D3D11_FILL_MODE fillMode, D3D11_CULL_MODE cullMode);
	ID3D11RasterizerState* CreateRasterizer(ID3D11Device* gDevice, D3D11_FILL_MODE fillMode, D3D11_CULL_MODE cullMode, bool frontCounterClockwise);

	ID3D11ShaderResourceView* GetShaderResourceView(int);

	int d_textureWidth, d_textureHeight;

	ID3D11RasterizerState* gRasteriserState;

	ID3D11Texture2D* d_renderTargetTextureArray[BUFFER_COUNT];
	ID3D11RenderTargetView* d_renderTargetViewArray[BUFFER_COUNT];
	ID3D11ShaderResourceView* d_shaderResourceViewArray[BUFFER_COUNT];
	ID3D11Texture2D* d_depthStencilBuffer;
	ID3D11DepthStencilView* d_depthStencilView;
	ID3D11ShaderResourceView* d_depthResourceView;
	ID3D11DepthStencilState* d_depthStencilState;
	D3D11_VIEWPORT d_viewPort;

};


#endif DEFERREDBUFFERCLASS_H
