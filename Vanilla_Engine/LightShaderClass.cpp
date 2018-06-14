
#include "LightShaderClass.h"

#include "ApplicationCore.h"

LightShaderClass::LightShaderClass() {

	l_vertexShader = nullptr;
	l_pixelShader = nullptr;
	l_layout = nullptr;
	l_sampleState = nullptr;
	l_lightBuffer = nullptr;
}

LightShaderClass::~LightShaderClass() {


}

void LightShaderClass::Shutdown() {

	SAFE_RELEASE(l_vertexShader);
	SAFE_RELEASE(l_pixelShader);
	SAFE_RELEASE(l_layout);
	SAFE_RELEASE(l_sampleState);

	// Light manager releases the light buffer
	//SAFE_RELEASE(l_lightBuffer);
}

bool LightShaderClass::Initialize(ID3D11Device* gDevice) {

	if (!InitializeShader(gDevice)) {

		MessageBox(
			NULL,
			L"CRITICAL ERROR: Light Shaders couldn't be initialized\nClosing application...",
			L"ERROR",
			MB_OK);
	}

	if (!CreatePointSampler(gDevice)) {

		MessageBox(
			NULL,
			L"CRITICAL ERROR: Light Texture Sampler couldn't be created\nClosing application...",
			L"ERROR",
			MB_OK);
	}

	// Set the light buffer from the LightManager
	SetLightBuffer();

	return true;
}

bool LightShaderClass::InitializeShader(ID3D11Device* gDevice) {

	HRESULT hr;

	D3D11_INPUT_ELEMENT_DESC vertexInputLayout[2];

	ID3DBlob* vsBlob = nullptr;
	ID3DBlob* vsErrorBlob = nullptr;

	hr = D3DCompileFromFile(
		L"Shaders\\LightShaders\\LightVS.hlsl",
		nullptr,
		nullptr,
		"VS_main",
		"vs_5_0",
		D3DCOMPILE_DEBUG,
		0,
		&vsBlob,
		&vsErrorBlob
	);

	if (FAILED(hr)) {

		cout << "Light Vertex Shader Error: Vertex Shader could not be compiled or loaded from file" << endl;

		if (vsErrorBlob) {

			OutputDebugStringA((char*)vsErrorBlob->GetBufferPointer());
			vsErrorBlob->Release();
		}

		return false;
	}


	hr = gDevice->CreateVertexShader(vsBlob->GetBufferPointer(), vsBlob->GetBufferSize(), nullptr, &l_vertexShader);

	if (FAILED(hr)) {

		cout << "Light Vertex Shader Error: Vertex Shader could not be created" << endl;
		return false;
	}

	setDebugName(l_vertexShader, "DEFERRED_LIGHT_VERTEX_SHADER");

	vertexInputLayout[0].SemanticName = "POSITION";
	vertexInputLayout[0].SemanticIndex = 0;
	vertexInputLayout[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	vertexInputLayout[0].InputSlot = 0;
	vertexInputLayout[0].AlignedByteOffset = 0;
	vertexInputLayout[0].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	vertexInputLayout[0].InstanceDataStepRate = 0;

	vertexInputLayout[1].SemanticName = "TEXCOORD";
	vertexInputLayout[1].SemanticIndex = 0;
	vertexInputLayout[1].Format = DXGI_FORMAT_R32G32_FLOAT;
	vertexInputLayout[1].InputSlot = 0;
	vertexInputLayout[1].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	vertexInputLayout[1].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	vertexInputLayout[1].InstanceDataStepRate = 0;

	int inputLayoutSize = sizeof(vertexInputLayout) / sizeof(vertexInputLayout[0]);
	gDevice->CreateInputLayout(vertexInputLayout, inputLayoutSize, vsBlob->GetBufferPointer(), vsBlob->GetBufferSize(), &l_layout);

	if (FAILED(hr)) {

		cout << "Light Vertex Shader Error: Shader Input Layout could not be created" << endl;
	}

	vsBlob->Release();

	setDebugName(l_layout, "DEFERRED_LIGHT_INPUT_LAYOUT");

	ID3DBlob* psBlob = nullptr;
	ID3DBlob* psErrorBlob = nullptr;

	hr = D3DCompileFromFile(
		L"Shaders\\LightShaders\\LightPS.hlsl",
		nullptr,
		nullptr,
		"PS_main",
		"ps_5_0",
		D3DCOMPILE_DEBUG,
		0,
		&psBlob,
		&psErrorBlob
	);

	if (FAILED(hr)) {

		cout << "Light Pixel Shader Error: Light Pixel Shader could not be compiled or loaded from file" << endl;

		if (psErrorBlob) {

			OutputDebugStringA((char*)psErrorBlob->GetBufferPointer());
			psErrorBlob->Release();
		}

		return false;
	}

	hr = gDevice->CreatePixelShader(psBlob->GetBufferPointer(), psBlob->GetBufferSize(), nullptr, &l_pixelShader);

	if (FAILED(hr)) {

		cout << "Light Pixel Shader Error: Pixel Shader could not be created" << endl;
		return false;
	}

	psBlob->Release();

	setDebugName(l_pixelShader, "DEFERRED_LIGHT_PIXEL_SHADER");

	return true;

}

bool LightShaderClass::CreatePointSampler(ID3D11Device* gDevice) {

	HRESULT hr;
	D3D11_SAMPLER_DESC samplerDesc;

	// Create a texture sampler state description.
	samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
	samplerDesc.MipLODBias = 0.0f;
	samplerDesc.MaxAnisotropy = 1;
	samplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	samplerDesc.BorderColor[0] = 0;
	samplerDesc.BorderColor[1] = 0;
	samplerDesc.BorderColor[2] = 0;
	samplerDesc.BorderColor[3] = 0;
	samplerDesc.MinLOD = 0;
	samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

	// Create the texture sampler state.
	hr = gDevice->CreateSamplerState(&samplerDesc, &l_sampleState);
	
	if (FAILED(hr))
	{
		return false;
	}

	setDebugName(l_sampleState, "DEFERRED_POINT_SAMPLER");

	return true;
}

void LightShaderClass::SetLightBuffer() {

	this->l_lightBuffer = ApplicationCore::GetInstance()->GetLightManager()->GetLightBuffer();
}

bool LightShaderClass::SetShaderParameters(ID3D11DeviceContext* gDeviceContext, ID3D11ShaderResourceView* colorTexture, ID3D11ShaderResourceView* normalTexture, ID3D11ShaderResourceView* worldTexture, ID3D11ShaderResourceView* depthTexture) {

	gDeviceContext->PSSetShaderResources(0, 1, &colorTexture);
	gDeviceContext->PSSetShaderResources(1, 1, &normalTexture);
	gDeviceContext->PSSetShaderResources(2, 1, &worldTexture);
	gDeviceContext->PSSetShaderResources(3, 1, &depthTexture);

	gDeviceContext->PSSetConstantBuffers(0, 1, &l_lightBuffer);

	return true;

}

void LightShaderClass::Render(ID3D11DeviceContext* gDeviceContext) {

	// No input layout necessary since we're using vertexID in the HLSL shader
	//gDeviceContext->IASetInputLayout(l_layout);

	gDeviceContext->VSSetShader(l_vertexShader, NULL, 0);
	gDeviceContext->PSSetShader(l_pixelShader, NULL, 0);

	gDeviceContext->PSSetSamplers(0, 1, &l_sampleState);

	gDeviceContext->Draw(3, 0);
}