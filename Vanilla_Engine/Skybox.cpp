
#include "Skybox.h"

#include "ApplicationCore.h"

Skybox::Skybox() {


}

Skybox::~Skybox() {


}

void Skybox::Initialize(ID3D11Device* gDevice) {

	if (!InitializeSkyboxShaders(gDevice)) {

		MessageBox(
			NULL,
			L"CRITICAL ERROR: Skybox shaders could not be initialized\nClosing application...",
			L"ERROR",
			MB_OK);

		PostQuitMessage(0);
	}

	skyboxTransform.SetScaling(XMFLOAT3(1, 1, 1));
	skyboxTransform = ApplicationCore::GetInstance()->GetTransformManager()->CreateTransformBuffer();
	cubemap = ApplicationCore::GetInstance()->GetMaterialManager()->CreateDDSTexture(L"Textures//Skyboxes//SunSky.dds", 1024);
	sphere = ApplicationCore::GetInstance()->GetMeshManager()->GenerateSphere(400, 400, 400);
	skyboxSampler = ApplicationCore::GetInstance()->GetMaterialManager()->CreateTextureSampler(D3D11_FILTER_MIN_MAG_MIP_LINEAR);

}

void Skybox::Shutdown() {

	SAFE_RELEASE(skyVertexShader);
	SAFE_RELEASE(skyPixelShader);
	SAFE_RELEASE(skyLayout);
}

bool Skybox::InitializeSkyboxShaders(ID3D11Device* gDevice) {

	HRESULT hr;
	D3D11_INPUT_ELEMENT_DESC vertexInputLayout[1];

	ID3DBlob* vsBlob = nullptr;
	ID3DBlob* vsErrorBlob = nullptr;

	hr = D3DCompileFromFile(
		L"Shaders\\Skybox\\SkyboxVS.hlsl",
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

	hr = gDevice->CreateVertexShader(vsBlob->GetBufferPointer(), vsBlob->GetBufferSize(), nullptr, &skyVertexShader);

	if (FAILED(hr)) {

		cout << "Vertex Shader Error: Vertex Shader could not be created" << endl;
		return false;
	}

	setDebugName(skyVertexShader, "SKYBOX_VERTEX_SHADER");

	vertexInputLayout[0].SemanticName = "POSITION";
	vertexInputLayout[0].SemanticIndex = 0;
	vertexInputLayout[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	vertexInputLayout[0].InputSlot = 0;
	vertexInputLayout[0].AlignedByteOffset = 0;
	vertexInputLayout[0].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	vertexInputLayout[0].InstanceDataStepRate = 0;

	int inputLayoutSize = sizeof(vertexInputLayout) / sizeof(vertexInputLayout[0]);
	gDevice->CreateInputLayout(vertexInputLayout, inputLayoutSize, vsBlob->GetBufferPointer(), vsBlob->GetBufferSize(), &skyLayout);

	if (FAILED(hr)) {

		cout << "Vertex Shader Error: Shader Input Layout could not be created" << endl;
	}

	vsBlob->Release();

	setDebugName(skyLayout, "SKYBOX_INPUT_LAYOUT");

	ID3DBlob* psBlob = nullptr;
	ID3DBlob* psErrorBlob = nullptr;

	hr = D3DCompileFromFile(
		L"Shaders\\Skybox\\SkyboxPS.hlsl",
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

	hr = gDevice->CreatePixelShader(psBlob->GetBufferPointer(), psBlob->GetBufferSize(), nullptr, &skyPixelShader);

	if (FAILED(hr)) {

		cout << "Pixel Shader Error: Pixel Shader could not be created" << endl;
		return false;
	}

	setDebugName(skyPixelShader, "SKYBOX_PIXEL_SHADER");

	psBlob->Release();

	return true;
}

void Skybox::LoadSkybox(const wchar_t* filename, const int maxSize) {

	cubemap = ApplicationCore::GetInstance()->GetMaterialManager()->CreateDDSTexture(filename, maxSize);
}

void Skybox::SetTextureSampler(ID3D11SamplerState* samplerState) {

	this->skyboxSampler = samplerState;
}

void Skybox::Update() {
	
	Camera* cam = ApplicationCore::GetInstance()->GetCamera();
	
	// Define the sphere world space matrix
	XMFLOAT4 camPos = cam->GetPosition();
	XMMATRIX sphereTranslation = XMMatrixTranslation(camPos.x, camPos.y, camPos.z);
	XMMATRIX sphereScaling = skyboxTransform.GetScalingMatrix();

	XMMATRIX sphereWorld = XMMatrixMultiply(sphereTranslation, sphereScaling);
	XMVECTOR determinant = XMMatrixDeterminant(sphereWorld);

	ZeroMemory(&skyboxMappedResource, sizeof(skyboxMappedResource));
	ApplicationCore::GetInstance()->GetDevices()->GetDeviceContext()->Map(skyboxTransform.transformBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &skyboxMappedResource);

	TRANSFORM_BUFFER* bufferPointer = (TRANSFORM_BUFFER*)skyboxMappedResource.pData;

	XMStoreFloat4x4(&bufferPointer->matrixW, XMMatrixTranspose(sphereWorld));
	XMStoreFloat4x4(&bufferPointer->entityInvTranspose, XMMatrixTranspose(XMMatrixInverse(&determinant, sphereWorld)));

	ApplicationCore::GetInstance()->GetDevices()->GetDeviceContext()->Unmap(skyboxTransform.transformBuffer, 0);

}

void Skybox::Render(ID3D11DeviceContext* gDeviceContext) {

	//Set vertex buffer stride and offset
	unsigned int stride = sizeof(Vertex);
	unsigned int offset = 0;
	
	gDeviceContext->VSSetShader(skyVertexShader, nullptr, 0);
	gDeviceContext->PSSetShader(skyPixelShader, nullptr, 0);
	
	gDeviceContext->VSSetConstantBuffers(0, 1, ApplicationCore::GetInstance()->GetConstBuffer());
	gDeviceContext->VSSetConstantBuffers(1, 1, &skyboxTransform.transformBuffer);
	gDeviceContext->PSSetShaderResources(0, 1, &cubemap);
	
	gDeviceContext->PSSetSamplers(0, 1, &skyboxSampler);
	
	// Set the vertex buffer for the input assembler
	gDeviceContext->IASetVertexBuffers(0, 1, &sphere.vertexBuffer, &stride, &offset);
	
	// Set the index buffer for the input assembler
	gDeviceContext->IASetIndexBuffer(sphere.indexBuffer, DXGI_FORMAT_R32_UINT, 0);
	
	gDeviceContext->IASetInputLayout(skyLayout);
	
	// Set the primitive topology to line list
	gDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	gDeviceContext->DrawIndexed(sphere.indices.size(), 0, 0);
}