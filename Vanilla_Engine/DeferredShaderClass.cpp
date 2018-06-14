
#include "DeferredShaderClass.h"

DeferredShaderClass::DeferredShaderClass() {

	d_vertexShader = nullptr;
	d_pixelShader = nullptr;
	d_layout = nullptr;
}

DeferredShaderClass::~DeferredShaderClass() {


}

void DeferredShaderClass::Shutdown() {

	SAFE_RELEASE(d_vertexShader);
	SAFE_RELEASE(d_pixelShader);
	SAFE_RELEASE(d_layout);
}

bool DeferredShaderClass::InitializeStandardShader(ID3D11Device* gDevice) {

	HRESULT hr;
	D3D11_INPUT_ELEMENT_DESC vertexInputLayout[3];

	ID3DBlob* vsBlob = nullptr;
	ID3DBlob* vsErrorBlob = nullptr;

	hr = D3DCompileFromFile(
		L"Shaders\\DeferredShader_Default\\DeferredVertex_Default.hlsl",
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

		cout << "Vertex Shader Error: Vertex Shader could not be compiled or loaded from file" << endl;

		if (vsErrorBlob) {

			OutputDebugStringA((char*)vsErrorBlob->GetBufferPointer());
			vsErrorBlob->Release();
		}

		return false;
	}

	hr = gDevice->CreateVertexShader(vsBlob->GetBufferPointer(), vsBlob->GetBufferSize(), nullptr, &d_vertexShader);

	if (FAILED(hr)) {

		cout << "Vertex Shader Error: Vertex Shader could not be created" << endl;
		return false;
	}

	setDebugName(d_vertexShader, "DEFERRED_STANDARD_VERTEX_SHADER");

	vertexInputLayout[0].SemanticName = "POSITION";
	vertexInputLayout[0].SemanticIndex = 0;
	vertexInputLayout[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	vertexInputLayout[0].InputSlot = 0;
	vertexInputLayout[0].AlignedByteOffset = 0;
	vertexInputLayout[0].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	vertexInputLayout[0].InstanceDataStepRate = 0;

	vertexInputLayout[1].SemanticName = "NORMAL";
	vertexInputLayout[1].SemanticIndex = 0;
	vertexInputLayout[1].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	vertexInputLayout[1].InputSlot = 0;
	vertexInputLayout[1].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	vertexInputLayout[1].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	vertexInputLayout[1].InstanceDataStepRate = 0;

	vertexInputLayout[2].SemanticName = "TEXCOORD";
	vertexInputLayout[2].SemanticIndex = 0;
	vertexInputLayout[2].Format = DXGI_FORMAT_R32G32_FLOAT;
	vertexInputLayout[2].InputSlot = 0;
	vertexInputLayout[2].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	vertexInputLayout[2].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	vertexInputLayout[2].InstanceDataStepRate = 0;

	int inputLayoutSize = sizeof(vertexInputLayout) / sizeof(vertexInputLayout[0]);
	gDevice->CreateInputLayout(vertexInputLayout, inputLayoutSize, vsBlob->GetBufferPointer(), vsBlob->GetBufferSize(), &d_layout);

	if (FAILED(hr)) {

		cout << "Vertex Shader Error: Shader Input Layout could not be created" << endl;
	}

	vsBlob->Release();

	setDebugName(d_layout, "DEFERRED_STANDARD_INPUT_LAYOUT");

	ID3DBlob* psBlob = nullptr;
	ID3DBlob* psErrorBlob = nullptr;

	hr = D3DCompileFromFile(
		L"Shaders\\DeferredShader_Default\\DeferredFragment_Default.hlsl",
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

		cout << "Pixel Shader Error: Pixel Shader could not be compiled or loaded from file" << endl;

		if (psErrorBlob) {

			OutputDebugStringA((char*)psErrorBlob->GetBufferPointer());
			psErrorBlob->Release();
		}

		return false;
	}

	hr = gDevice->CreatePixelShader(psBlob->GetBufferPointer(), psBlob->GetBufferSize(), nullptr, &d_pixelShader);

	if (FAILED(hr)) {

		cout << "Pixel Shader Error: Pixel Shader could not be created" << endl;
		return false;
	}

	setDebugName(d_pixelShader, "DEFERRED_STANDARD_PIXEL_SHADER");

	psBlob->Release();

	return true;
}

void DeferredShaderClass::Render(ID3D11DeviceContext* gDeviceContext, int indexCount) {

	// Set the vertex input layout
	gDeviceContext->IASetInputLayout(d_layout);

	// Set the vertex and pixel shaders
	gDeviceContext->VSSetShader(d_vertexShader, nullptr, 0);
	gDeviceContext->PSSetShader(d_pixelShader, nullptr, 0);

	// Render the geometry
	gDeviceContext->Draw(indexCount, 0);
}
