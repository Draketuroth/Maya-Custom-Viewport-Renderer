
#include "Grid.h"

#include "ApplicationCore.h"

GridClass::GridClass() {

	
}

GridClass::~GridClass() {


}

void GridClass::Shutdown() {

	SAFE_RELEASE(gridVertexBuffer);
	SAFE_RELEASE(gridIndexBuffer);

	SAFE_RELEASE(gridVertexLayout);
	SAFE_RELEASE(gridVertexShader);
	SAFE_RELEASE(gridPixelShader);
}

void GridClass::Render(ID3D11DeviceContext* gDeviceContext) {

	// Set vertex buffer stride and offset
	unsigned int stride = sizeof(PointVertex);
	unsigned int offset = 0;

	gDeviceContext->VSSetShader(gridVertexShader, nullptr, 0);
	gDeviceContext->PSSetShader(gridPixelShader, nullptr, 0);

	gDeviceContext->VSSetConstantBuffers(0, 1, ApplicationCore::GetInstance()->GetConstBuffer());

	// Set the vertex input layout
	gDeviceContext->IASetInputLayout(gridVertexLayout);

	// Set the vertex buffer for the input assembler
	gDeviceContext->IASetVertexBuffers(0, 1, &gridVertexBuffer, &stride, &offset);

	// Set the index buffer for the input assembler
	gDeviceContext->IASetIndexBuffer(gridIndexBuffer, DXGI_FORMAT_R32_UINT, 0);

	// Set the primitive topology to line list
	gDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINELIST);

	gDeviceContext->DrawIndexed(indexCount, 0, 0);

}

int GridClass::GetIndexCount() {

	return indexCount;
}

HRESULT GridClass::Initialize(ID3D11Device* gDevice) {

	HRESULT hr = S_OK;

	gridWidth = 100;
	gridHeight = 100;

	hr = InitializeBuffers(gDevice, XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));
	if (FAILED(hr)) {

		return S_FALSE;
	}

	hr = CreateGridShader(gDevice);
	if (FAILED(hr)) {

		return S_FALSE;
	}

	return hr;
}

HRESULT GridClass::InitializeBuffers(ID3D11Device* gDevice, XMFLOAT4 color) {

	HRESULT hr;

	PointVertex* vertices;
	unsigned long* indices;

	int index, i, j;
	float posX, posZ;

	D3D11_BUFFER_DESC vertexBufferDesc, indexBufferDesc;
	D3D11_SUBRESOURCE_DATA vertexData, indexData;
	
	// Set the number of vertices in the mesh
	vertexCount = (gridWidth - 1) * (gridHeight - 1) * 8;
	indexCount = vertexCount;

	// Create a temporary vertex and index arrays

	vertices = new PointVertex[vertexCount];

	if (!vertices) {

		return S_FALSE;
	}

	indices = new unsigned long[indexCount];

	// Initialize the index to the vertex array
	index = 0;

	// Load the vertex and index arrays with the grid data

	for (j = 0; j < (gridHeight - 1); j++) {

		for (i = 0; i < (gridWidth - 1); i++) {

			// LINE 1
			// Upper left.
			posX = (float)i;
			posZ = (float)(j + 1);

			vertices[index].pos= XMFLOAT3(posX, 0.0f, posZ);
			vertices[index].color = color;
			indices[index] = index;
			index++;

			// Upper right.
			posX = (float)(i + 1);
			posZ = (float)(j + 1);

			vertices[index].pos = XMFLOAT3(posX, 0.0f, posZ);
			vertices[index].color = color;
			indices[index] = index;
			index++;

			// LINE 2
			// Upper right.
			posX = (float)(i + 1);
			posZ = (float)(j + 1);

			vertices[index].pos = XMFLOAT3(posX, 0.0f, posZ);
			vertices[index].color = color;
			indices[index] = index;
			index++;

			// Bottom right.
			posX = (float)(i + 1);
			posZ = (float)j;

			vertices[index].pos = XMFLOAT3(posX, 0.0f, posZ);
			vertices[index].color = color;
			indices[index] = index;
			index++;

			// LINE 3
			// Bottom right.
			posX = (float)(i + 1);
			posZ = (float)j;

			vertices[index].pos = XMFLOAT3(posX, 0.0f, posZ);
			vertices[index].color = color;
			indices[index] = index;
			index++;

			// Bottom left.
			posX = (float)i;
			posZ = (float)j;

			vertices[index].pos = XMFLOAT3(posX, 0.0f, posZ);
			vertices[index].color = color;
			indices[index] = index;
			index++;

			// LINE 4
			// Bottom left.
			posX = (float)i;
			posZ = (float)j;

			vertices[index].pos = XMFLOAT3(posX, 0.0f, posZ);
			vertices[index].color = color;
			indices[index] = index;
			index++;

			// Upper left.
			posX = (float)i;
			posZ = (float)(j + 1);

			vertices[index].pos = XMFLOAT3(posX, 0.0f, posZ);
			vertices[index].color = color;
			indices[index] = index;
			index++;

		}
	}

	// Description for the static vertex buffer
	vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	vertexBufferDesc.ByteWidth = sizeof(PointVertex) * vertexCount;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = 0;
	vertexBufferDesc.MiscFlags = 0;
	vertexBufferDesc.StructureByteStride = 0;

	// Give the subresource a pointer to the vertex data
	vertexData.pSysMem = vertices;
	vertexData.SysMemPitch = 0;
	vertexData.SysMemSlicePitch = 0;

	// Create the vertex buffer
	hr = gDevice->CreateBuffer(&vertexBufferDesc, &vertexData, &gridVertexBuffer);
	if (FAILED(hr)) {

		return S_FALSE;
	}

	// Description for the static index buffer
	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	indexBufferDesc.ByteWidth = sizeof(unsigned long) * indexCount;
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.CPUAccessFlags = 0;
	indexBufferDesc.MiscFlags = 0;
	indexBufferDesc.StructureByteStride = 0;

	// Give the subresource a pointer to the index data
	indexData.pSysMem = indices;
	indexData.SysMemPitch = 0;
	indexData.SysMemSlicePitch = 0;

	// Create the index buffer
	hr = gDevice->CreateBuffer(&indexBufferDesc, &indexData, &gridIndexBuffer);
	if (FAILED(hr)) {

		return S_FALSE;
	}

	// Release the arrays now that the buffers have been created and loaded
	delete[] vertices;
	vertices = 0;

	delete[] indices;
	indices = 0;

	return hr;

}

HRESULT GridClass::CreateGridShader(ID3D11Device* gDevice) {

	HRESULT hr = S_OK;

	ID3DBlob* vsBlob = nullptr;
	ID3DBlob* vsErrorBlob = nullptr;

	hr = D3DCompileFromFile(
		L"Shaders\\GridShaders\\GridVS.hlsl",
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

		cout << "Grid Vertex Shader Error: Vertex Shader could not be compiled or loaded from file" << endl;

		if (vsErrorBlob) {

			OutputDebugStringA((char*)vsErrorBlob->GetBufferPointer());
			vsErrorBlob->Release();
		}

		return S_FALSE;
	}


	hr = gDevice->CreateVertexShader(vsBlob->GetBufferPointer(), vsBlob->GetBufferSize(), nullptr, &gridVertexShader);

	if (FAILED(hr)) {

		cout << "Grid Vertex Shader Error: Vertex Shader could not be created" << endl;
		return S_FALSE;
	}

	setDebugName(gridVertexShader, "GRID_VERTEX_SHADER");

	D3D11_INPUT_ELEMENT_DESC vertexInputDesc[] = {

		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};

	int inputLayoutSize = sizeof(vertexInputDesc) / sizeof(D3D11_INPUT_ELEMENT_DESC);
	gDevice->CreateInputLayout(vertexInputDesc, inputLayoutSize, vsBlob->GetBufferPointer(), vsBlob->GetBufferSize(), &gridVertexLayout);

	if (FAILED(hr)) {

		cout << "Grid Vertex Shader Error: Shader Input Layout could not be created" << endl;
		return S_FALSE;
	}

	setDebugName(gridVertexLayout, "GRID_VERTEX_LAYOUT");

	vsBlob->Release();



	ID3DBlob* psBlob = nullptr;
	ID3DBlob* psErrorBlob = nullptr;

	hr = D3DCompileFromFile(
		L"Shaders\\GridShaders\\GridPS.hlsl",
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

		cout << "Grid Pixel Shader Error: Pixel Shader could not be compiled or loaded from file" << endl;

		if (psErrorBlob) {

			OutputDebugStringA((char*)psErrorBlob->GetBufferPointer());
			psErrorBlob->Release();
		}

		return S_FALSE;
	}

	hr = gDevice->CreatePixelShader(psBlob->GetBufferPointer(), psBlob->GetBufferSize(), nullptr, &gridPixelShader);

	if (FAILED(hr)) {

		cout << "Grid Pixel Shader Error: Pixel Shader could not be created" << endl;
		return S_FALSE;
	}

	setDebugName(gridPixelShader, "GRID_PIXEL_SHADER");

	psBlob->Release();

	return hr;
}

void GridClass::RenderBuffers(ID3D11DeviceContext* gDeviceContext) {

	// Set vertex buffer stride and offset
	unsigned int stride = sizeof(PointVertex);
	unsigned int offset = 0;

	// Set the vertex buffer for the input assembler
	gDeviceContext->IASetVertexBuffers(0, 1, &gridVertexBuffer, &stride, &offset);

	// Set the index buffer for the input assembler
	gDeviceContext->IASetIndexBuffer(gridIndexBuffer, DXGI_FORMAT_R32_UINT, 0);

	// Set the primitive topology to line list
	gDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINELIST);

}



