
#include "Renderer.h"

#include "ApplicationCore.h"

Renderer::Renderer() {

	
}

Renderer::~Renderer() {


}

void Renderer::Initialize(ID3D11Device* device, ID3D11DeviceContext* deviceContext) {

	grid.Initialize(device);

	skybox.Initialize(device);

	SetTextureSampler(ApplicationCore::GetInstance()->GetMaterialManager()->CreateTextureSampler(D3D11_FILTER_MIN_MAG_MIP_LINEAR));

	deferredCore.Initialize(device);

	lightShaders.Initialize(device);

	CreateShaders(device);

	this->deviceContext = deviceContext;
	
}

void Renderer::Shutdown() {

	skybox.Shutdown();
	grid.Shutdown();
	deferredCore.Shutdown();
	deferredShaders.Shutdown();
	lightShaders.Shutdown();
}

void Renderer::SetTextureSampler(ID3D11SamplerState* samplerState) {

	this->texSampler = samplerState;
}

void Renderer::Update() {


}

void Renderer::Clear() {

	// clear the back buffer to a deep blue
	float clearColor[] = { 0, 0, 0, 1 };	// Back buffer clear color as an array of floats (rgba)

	// Clear the primary render target view using the specified color
	ApplicationCore::GetInstance()->GetDevices()->GetDeviceContext()->ClearRenderTargetView(
	ApplicationCore::GetInstance()->GetDevices()->GetRTV(),
	clearColor);

	// Clear the standard depth stencil view
	ApplicationCore::GetInstance()->GetDevices()->GetDeviceContext()->ClearDepthStencilView(
	deferredCore.d_depthStencilView, 
	D3D11_CLEAR_DEPTH, 
	1.0f, 
	0);

	// Clear the shadow map render target view using the specified color
}

void Renderer::RenderLightPass() {

	// Step 2: Unbinding

	ID3D11RenderTargetView* nullRenderTargets = { nullptr };
	ID3D11DepthStencilView* nullDepthStencilView = { nullptr };
	ApplicationCore::GetInstance()->GetDevices()->GetDeviceContext()->OMSetRenderTargets(1, &nullRenderTargets, nullDepthStencilView);

	ID3D11ShaderResourceView* nullShaderResourceView = { nullptr };
	ApplicationCore::GetInstance()->GetDevices()->GetDeviceContext()->PSSetShaderResources(0, 1, &nullShaderResourceView);

	// Step 3: Switch back to backbuffer as render target
	// Turn the render target back to the original back buffer and not the render buffers anymore
	// Turns off the z-buffer for 2D rendering
	ResetRenderTarget();

	// Step 4: 2D rendering of light calculations

	lightShaders.SetShaderParameters(ApplicationCore::GetInstance()->GetDevices()->GetDeviceContext(),
		deferredCore.d_shaderResourceViewArray[0],
		deferredCore.d_shaderResourceViewArray[1],
		deferredCore.d_shaderResourceViewArray[2],
		deferredCore.d_depthResourceView);

	ApplicationCore::GetInstance()->GetDevices()->GetDeviceContext()->PSSetConstantBuffers(1, 1, ApplicationCore::GetInstance()->GetConstBuffer());

	lightShaders.Render(ApplicationCore::GetInstance()->GetDevices()->GetDeviceContext());

	ClearState();
	
}

bool Renderer::ResetDeferred() {

	// Set the render buffers to be the render target
	deferredCore.SetRenderTargets(deviceContext);

	// Clear the render buffers
	deferredCore.ClearRenderTargets(deviceContext, 0.0f, 0.0f, 0.0f, 1.0f);

	RenderSkybox(deviceContext);

	RenderGrid(deviceContext);

	return true;
}

void Renderer::ClearState() {

	// Clear state for the next render call
	ApplicationCore::GetInstance()->GetDevices()->GetDeviceContext()->ClearState();
}

void Renderer::ClearShaders() {

	ID3D11VertexShader* nullVS = nullptr;
	ID3D11GeometryShader* nullGS = nullptr;
	ID3D11PixelShader* nullPS = nullptr;

	// Set the vertex and pixel shaders
	ApplicationCore::GetInstance()->GetDevices()->GetDeviceContext()->VSSetShader(nullVS, nullptr, 0);
	ApplicationCore::GetInstance()->GetDevices()->GetDeviceContext()->GSSetShader(nullGS, nullptr, 0);
	ApplicationCore::GetInstance()->GetDevices()->GetDeviceContext()->PSSetShader(nullPS, nullptr, 0);
}

void Renderer::RenderSkybox(ID3D11DeviceContext* gDeviceContext) {

	skybox.Update();
	skybox.Render(gDeviceContext);
}

void Renderer::RenderGrid(ID3D11DeviceContext* gDeviceContext) {

	grid.Render(gDeviceContext);
}

void Renderer::SetPrimaryRenderTarget() {

	Clear();

	ApplicationCore::GetInstance()->GetDevices()->GetDeviceContext()->OMSetRenderTargets(
	1,
	ApplicationCore::GetInstance()->GetDevices()->GetRenderTarget(),
	deferredCore.d_depthStencilView);
}

bool Renderer::RenderStaticObjectToTexture(string ID) {

	if(ApplicationCore::GetInstance()->FindEntity(ID)){

		Components& components = ApplicationCore::GetInstance()->GetEntity(ID).GetComponents();
		Mesh_Standard& mesh = components.GetStaticMesh();

		// Get the number of vertices
		int indexCounter = mesh.GetVertices().size();
		
		if(indexCounter > 0){

		// Set WVP constant buffer
		deviceContext->VSSetConstantBuffers(0, 1, ApplicationCore::GetInstance()->GetConstBuffer());

		// Set entity transform buffer
		deviceContext->VSSetConstantBuffers(1, 1, &components.GetTransform().transformBuffer);
		
		// Set entity material buffer
		deviceContext->PSSetConstantBuffers(0, 1, &components.GetMaterial()->materialBuffer);

		// Set entity texture
		ID3D11ShaderResourceView* view[1] = { components.GetMaterial()->GetTexture() };
		deviceContext->PSSetShaderResources(0, 1, &view[0]);

		// Set texture sampler
		deviceContext->PSSetSamplers(0, 1, &texSampler);

		// Set entity vertex buffer
		deferredCore.SetStaticObjectBuffer(deviceContext, mesh.vertexBuffer);

		// Render the object
		deferredShaders.Render(deviceContext, indexCounter);

		return true;

		}

		else {

			cout << "Entity " << ID << " had no vertices" << endl;
			return true;
		}

	}

	else {

		return false;
	}
}

void Renderer::CreateShaders(ID3D11Device* gDevice) {

	if (!deferredShaders.InitializeStandardShader(gDevice)) {

		MessageBox(
			NULL,
			L"CRITICAL ERROR: Deferred standard shaders couldn't be initialized\nClosing application...",
			L"ERROR",
			MB_OK);
		PostQuitMessage(0);
	}

	if (!lightShaders.Initialize(gDevice)) {

		MessageBox(
			NULL,
			L"CRITICAL ERROR: Light shaders couldn't be initialized\nClosing application...",
			L"ERROR",
			MB_OK);
		PostQuitMessage(0);
	}

}

void Renderer::ResetRenderTarget() {

	ID3D11DepthStencilView* nullDepthView = { nullptr };
	ApplicationCore::GetInstance()->GetDevices()->GetDeviceContext()->OMSetRenderTargets(1, ApplicationCore::GetInstance()->GetDevices()->GetRenderTarget(), nullDepthView);
}