#ifndef GRID_H
#define GRID_H

#include "DebugHelper.h"
#include "VertexTypes.h"
#include <iostream>

class GridClass {

public:
	GridClass();
	~GridClass();

	void Shutdown();
	HRESULT Initialize(ID3D11Device* gDevice);

	void Render(ID3D11DeviceContext* gDeviceContext);

	int GetIndexCount();

private:

	HRESULT InitializeBuffers(ID3D11Device* gDevice, XMFLOAT4 color);
	HRESULT CreateGridShader(ID3D11Device* gDevice);
	void RenderBuffers(ID3D11DeviceContext* gDeviceContext);

	int gridWidth, gridHeight;
	int vertexCount, indexCount;

	ID3D11Buffer* gridVertexBuffer;
	ID3D11Buffer* gridIndexBuffer;

	ID3D11InputLayout* gridVertexLayout;
	ID3D11VertexShader* gridVertexShader;
	ID3D11PixelShader* gridPixelShader;

};

#endif
