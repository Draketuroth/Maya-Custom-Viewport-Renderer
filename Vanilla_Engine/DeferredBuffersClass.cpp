
#include "DeferredBuffersClass.h"

#include "ApplicationCore.h"

DeferredBuffersClass::DeferredBuffersClass() {

	int i;

	for (i = 0; i < BUFFER_COUNT; i++) {

		d_renderTargetTextureArray[i] = nullptr;
		d_renderTargetViewArray[i] = nullptr;
		d_shaderResourceViewArray[i] = nullptr;
	}

	d_depthStencilBuffer = nullptr;
	d_depthStencilState = nullptr;
	d_depthStencilView = nullptr;
}

DeferredBuffersClass::~DeferredBuffersClass() {


}

void DeferredBuffersClass::Shutdown() {

	int i;

	SAFE_RELEASE(d_depthStencilView);
	SAFE_RELEASE(d_depthStencilState);
	SAFE_RELEASE(d_depthStencilBuffer);
	SAFE_RELEASE(d_depthResourceView);
	SAFE_RELEASE(gRasteriserState);

	for (i = 0; i < BUFFER_COUNT; i++) {

		SAFE_RELEASE(d_shaderResourceViewArray[i]);
		SAFE_RELEASE(d_renderTargetViewArray[i]);
		SAFE_RELEASE(d_renderTargetTextureArray[i]);
	}
}

bool DeferredBuffersClass::Initialize(ID3D11Device* gDevice) {

	if (!SetupResources(gDevice)) {

		MessageBox(
			NULL,
			L"CRITICAL ERROR: Deferred Resources couldn't be initialized, investigate SetupResources function\nClosing application...",
			L"ERROR",
			MB_OK);

			return false;
	
	}


	if (!CreateDepthStencil(gDevice)) {

		MessageBox(
			NULL,
			L"CRITICAL ERROR: Depth Stencil resources couldn't be initialized, investigate CreateDepthStencil function\nClosing application...",
			L"ERROR",
			MB_OK);

			return false;

	}

	CreateRasterizerState(gDevice, D3D11_FILL_SOLID, D3D11_CULL_NONE);

	CreateViewport();

	return true;
}

bool DeferredBuffersClass::SetupResources(ID3D11Device* gDevice) {

	HRESULT hr;
	UINT i;

	d_textureWidth = ApplicationCore::GetInstance()->GetWindow()->GetWindowWidth();
	d_textureHeight = ApplicationCore::GetInstance()->GetWindow()->GetWindowHeight();

	//----------------------------------------------------------------------------------------------------------------------------------//
	// TEXTURE DESCRIPTION
	//----------------------------------------------------------------------------------------------------------------------------------//

	D3D11_TEXTURE2D_DESC texDesc;
	ZeroMemory(&texDesc, sizeof(texDesc));

	texDesc.Width = d_textureWidth;
	texDesc.Height = d_textureHeight;
	texDesc.MipLevels = 1;
	texDesc.ArraySize = 1;
	texDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	texDesc.SampleDesc.Count = 1;
	texDesc.Usage = D3D11_USAGE_DEFAULT;
	texDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;

	for (i = 0; i < BUFFER_COUNT; i++) {

		hr = gDevice->CreateTexture2D(&texDesc, NULL, &d_renderTargetTextureArray[i]);

		if (FAILED(hr)) {

			return false;
		}

		setDebugName(d_renderTargetTextureArray[i], "DEFERRED_RENDER_TARGET_TEXTURE" + i);
	}

	//----------------------------------------------------------------------------------------------------------------------------------//
	// RENDER TARGET VIEW DESCRIPTION
	//----------------------------------------------------------------------------------------------------------------------------------//

	D3D11_RENDER_TARGET_VIEW_DESC rtvDesc;
	ZeroMemory(&rtvDesc, sizeof(rtvDesc));

	rtvDesc.Format = texDesc.Format;
	rtvDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
	rtvDesc.Texture2D.MipSlice = 0;

	for (i = 0; i < BUFFER_COUNT; i++) {

		hr = gDevice->CreateRenderTargetView(d_renderTargetTextureArray[i], &rtvDesc, &d_renderTargetViewArray[i]);

		if (FAILED(hr)) {

			return false;
		}

		setDebugName(d_renderTargetTextureArray[i], "DEFERRED_RENDER_TARGET_VIEW" + i);
	}

	//----------------------------------------------------------------------------------------------------------------------------------//
	// SHADER RESOURCE VIEW DESCRIPTION
	//----------------------------------------------------------------------------------------------------------------------------------//

	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
	ZeroMemory(&srvDesc, sizeof(srvDesc));

	srvDesc.Format = texDesc.Format;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MostDetailedMip = 0;
	srvDesc.Texture2D.MipLevels = 1;

	for (i = 0; i < BUFFER_COUNT; i++) {

		hr = gDevice->CreateShaderResourceView(d_renderTargetTextureArray[i], &srvDesc, &d_shaderResourceViewArray[i]);

		if (FAILED(hr)) {

			return false;
		}

		setDebugName(d_renderTargetTextureArray[i], "DEFERRED_SHADER_RESOURCE_VIEW" + i);
	}

	return true;
}

bool DeferredBuffersClass::CreateDepthStencil(ID3D11Device* gDevice){
	
	HRESULT hr;
	DXGI_FORMAT resFormat = DXGI_FORMAT_R32G8X24_TYPELESS;
	DXGI_FORMAT srvFormat = DXGI_FORMAT_R32_FLOAT_X8X24_TYPELESS;

	//----------------------------------------------------------------------------------------------------------------------------------//
	// DEPTH TEXTURE DESCRIPTION
	//----------------------------------------------------------------------------------------------------------------------------------//

	// THe depth buffer texture is used to store the distance of each fragment to the camera

	D3D11_TEXTURE2D_DESC texDesc;
	ZeroMemory(&texDesc, sizeof(texDesc));

	texDesc.Width = d_textureWidth;
	texDesc.Height = d_textureHeight;
	texDesc.MipLevels = 1;
	texDesc.ArraySize = 1;
	texDesc.Format = resFormat;
	texDesc.SampleDesc.Count = 1;
	texDesc.SampleDesc.Quality = 0;
	texDesc.Usage = D3D11_USAGE_DEFAULT;
	texDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;
	texDesc.CPUAccessFlags = 0;
	texDesc.MiscFlags = 0;

	hr = gDevice->CreateTexture2D(&texDesc, nullptr, &d_depthStencilBuffer);

	if (FAILED(hr)) {

		return false;
	}

	setDebugName(d_depthStencilBuffer, "DEFERRED_DEPTH_STENCIL_TEXTURE2D");

	//----------------------------------------------------------------------------------------------------------------------------------//
	// DEPTH VIEW DESCRIPTION
	//----------------------------------------------------------------------------------------------------------------------------------//

	D3D11_DEPTH_STENCIL_VIEW_DESC dViewDesc;
	ZeroMemory(&dViewDesc, sizeof(dViewDesc));

	dViewDesc.Format = DXGI_FORMAT_D32_FLOAT_S8X24_UINT;
	dViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	dViewDesc.Texture2D.MipSlice = 0;
	dViewDesc.Flags = 0;

	hr = gDevice->CreateDepthStencilView(d_depthStencilBuffer, &dViewDesc, &d_depthStencilView);

	if (FAILED(hr)) {

		return false;
	}

	setDebugName(d_depthStencilView, "DEFERRED_DEPTH_STENCIL_VIEW");

	//----------------------------------------------------------------------------------------------------------------------------------//
	// RESOURCE VIEW DESCRIPTION
	//----------------------------------------------------------------------------------------------------------------------------------//

	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.Format = srvFormat;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MipLevels = texDesc.MipLevels;

	hr = gDevice->CreateShaderResourceView(d_depthStencilBuffer, &srvDesc, &d_depthResourceView);
	if (FAILED(hr))
	{
		return false;
	}

	setDebugName(d_depthResourceView, "DEFERRED_DEPTH_RESOURCE_VIEW");

	//----------------------------------------------------------------------------------------------------------------------------------//
	// DEPTH STATE DESCRIPTION
	//----------------------------------------------------------------------------------------------------------------------------------//

	D3D11_DEPTH_STENCIL_DESC stencilDesc;
	ZeroMemory(&stencilDesc, sizeof(stencilDesc));

	// Depth test
	stencilDesc.DepthEnable = true;
	stencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;

	// For Skybox rendering, Make sure the depth function is LESS_EQUAL and not just LESS.  
    // Otherwise, the normalized depth values at z = 1 (NDC) will 
    // fail the depth test if the depth buffer was cleared to 1.
	// Otherwise use D3D11_COMPARISON_LESS
	stencilDesc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;

	// Stencil test
	stencilDesc.StencilEnable = true;
	stencilDesc.StencilReadMask = 0xFF;
	stencilDesc.StencilWriteMask = 0xFF;

	// Stencil operations if the pixel is facing forward

	stencilDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	stencilDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
	stencilDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	stencilDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

	// Stencil operations if the pixel is facing backward

	stencilDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	stencilDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
	stencilDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	stencilDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

	// Create Depth State
	hr = gDevice->CreateDepthStencilState(&stencilDesc, &d_depthStencilState);

	if (FAILED(hr)) {

		return false;
	}

	setDebugName(d_depthStencilState, "DEFERRED_DEPTH_STENCIL_STATE");

	return true;

}

void DeferredBuffersClass::CreateViewport() {

	d_viewPort.MinDepth = 0.0f;
	d_viewPort.MaxDepth = 1.0f;
	d_viewPort.TopLeftX = 0.0f;
	d_viewPort.TopLeftY = 0.0f;
	d_viewPort.Width = (float)d_textureWidth;
	d_viewPort.Height = (float)d_textureHeight;

}

void DeferredBuffersClass::SetRenderTargets(ID3D11DeviceContext* gDeviceContext) {

	// Bind the render target view array and depth stencil buffer to the output render pipeline
	gDeviceContext->OMSetRenderTargets(BUFFER_COUNT, d_renderTargetViewArray, d_depthStencilView);

	// Set the viewport
	gDeviceContext->RSSetViewports(1, &d_viewPort);
}

void DeferredBuffersClass::SetRenderTarget(ID3D11DeviceContext* gDeviceContext, int GBufferIndex) {

	gDeviceContext->OMSetRenderTargets(BUFFER_COUNT, &d_renderTargetViewArray[GBufferIndex], d_depthStencilView);
}

void DeferredBuffersClass::SetViewPort(ID3D11DeviceContext* gDeviceContext) {

	// Set the viewport
	gDeviceContext->RSSetViewports(1, &d_viewPort);
}

void DeferredBuffersClass::ClearRenderTargets(ID3D11DeviceContext* gDeviceContext, float red, float green, float blue, float alpha) {

	float color[4];
	int i;

	// Setup the color to clear the buffer to
	color[0] = red;
	color[1] = green;
	color[2] = blue;
	color[3] = alpha;

	// Clear the render target buffers
	for (i = 0; i < BUFFER_COUNT; i++) {

		gDeviceContext->ClearRenderTargetView(d_renderTargetViewArray[i], color);

	}

	// Clear the depth buffer

	gDeviceContext->ClearDepthStencilView(d_depthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

}

void DeferredBuffersClass::SetStaticObjectBuffer(ID3D11DeviceContext* gDeviceContext, ID3D11Buffer* vertexBuffer) {

	UINT32 vertexSize = sizeof(Vertex);
	UINT32 offset = 0;
	gDeviceContext->IASetVertexBuffers(0, 1, &vertexBuffer, &vertexSize, &offset);
	gDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

}

bool DeferredBuffersClass::CreateRasterizerState(ID3D11Device* gDevice, D3D11_FILL_MODE fillMode, D3D11_CULL_MODE cullMode) {

	HRESULT hr;

	D3D11_RASTERIZER_DESC rasterizerDesc;
	ZeroMemory(&rasterizerDesc, sizeof(rasterizerDesc));

	rasterizerDesc.FillMode = fillMode;
	rasterizerDesc.CullMode = cullMode;
	rasterizerDesc.FrontCounterClockwise = true;

	rasterizerDesc.DepthBias = 0;
	rasterizerDesc.DepthBiasClamp = 0;
	rasterizerDesc.SlopeScaledDepthBias = 0;

	rasterizerDesc.DepthClipEnable = true;
	rasterizerDesc.ScissorEnable = false;
	rasterizerDesc.MultisampleEnable = false;
	rasterizerDesc.AntialiasedLineEnable = false;

	hr = gDevice->CreateRasterizerState(&rasterizerDesc, &gRasteriserState);

	if (FAILED(hr)) {

		MessageBox(
			NULL,
			L"CRITICAL ERROR: Rasterizer state could not be created\nClosing application...",
			L"ERROR",
			MB_OK);

		PostQuitMessage(0);
	}

	return true;
}

ID3D11RasterizerState* DeferredBuffersClass::CreateRasterizer(ID3D11Device* gDevice, D3D11_FILL_MODE fillMode, D3D11_CULL_MODE cullMode) {

	ID3D11RasterizerState* rState;

	HRESULT hr;

	D3D11_RASTERIZER_DESC rasterizerDesc;
	ZeroMemory(&rasterizerDesc, sizeof(rasterizerDesc));

	rasterizerDesc.FillMode = fillMode;
	rasterizerDesc.CullMode = cullMode;
	rasterizerDesc.FrontCounterClockwise = false;

	rasterizerDesc.DepthBias = 0;
	rasterizerDesc.DepthBiasClamp = 0;
	rasterizerDesc.SlopeScaledDepthBias = 0;

	rasterizerDesc.DepthClipEnable = true;
	rasterizerDesc.ScissorEnable = false;
	rasterizerDesc.MultisampleEnable = false;
	rasterizerDesc.AntialiasedLineEnable = false;

	hr = gDevice->CreateRasterizerState(&rasterizerDesc, &rState);

	if (FAILED(hr)) {

		MessageBox(
			NULL,
			L"CRITICAL ERROR: Rasterizer state could not be created\nClosing application...",
			L"ERROR",
			MB_OK);

		PostQuitMessage(0);
	}

	return rState;
}

ID3D11ShaderResourceView* DeferredBuffersClass::GetShaderResourceView(int view) {

	return d_shaderResourceViewArray[view];
}