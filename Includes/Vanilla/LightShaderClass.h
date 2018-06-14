#ifndef LIGHTSHADERCLASS_H
#define LIGHTSHADERCLASS_H

#include <d3d11.h>
#include <d3dcompiler.h>
#include <DirectXMath.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <iostream>

#include <vector>
#include "VertexTypes.h"

#include "MacroDefinitions.h"

#include "DebugHelper.h"

#include "Pointlight.h"

class LightShaderClass {

public:
	
	LightShaderClass();
	~LightShaderClass();
	void Shutdown();

	bool Initialize(ID3D11Device* gDevice);
	bool InitializeShader(ID3D11Device* gDevice);
	
	bool CreatePointSampler(ID3D11Device* gDevice);

	void SetLightBuffer();
	bool SetShaderParameters(ID3D11DeviceContext* gDeviceContext, ID3D11ShaderResourceView* colorTexture, ID3D11ShaderResourceView* normalTexture, ID3D11ShaderResourceView* worldTexture, ID3D11ShaderResourceView* depthTexture);
	void Render(ID3D11DeviceContext* gDeviceContext);

	ID3D11Buffer* const* GetLightBuffer() const { return &l_lightBuffer; };

private:

	ID3D11VertexShader* l_vertexShader;
	ID3D11PixelShader* l_pixelShader;
	ID3D11InputLayout* l_layout;
	ID3D11SamplerState* l_sampleState;
	ID3D11Buffer* l_lightBuffer;

};

#endif LIGHTSHADERCLASS_H
