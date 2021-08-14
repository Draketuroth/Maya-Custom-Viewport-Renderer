#ifndef SKYBOX_H
#define SKYBOX_H

#include "Datatypes.h"
#include "VertexTypes.h"
#include "DebugHelper.h"

#include "Mesh_Standard.h"
#include "Mesh_Standard.h"
#include "Transform.h"

#include <iostream>

class Skybox {

public:

	Skybox();
	~Skybox();

	void Initialize(ID3D11Device* gDevice);
	void Shutdown();

	bool InitializeSkyboxShaders(ID3D11Device* gDevice);
	void LoadSkybox(const wchar_t* filename, const int maxSize);

	void SetTextureSampler(ID3D11SamplerState* samplerState);

	void Update();
	void Render(ID3D11DeviceContext* gDeviceContext);

private:

	ID3D11ShaderResourceView* cubemap;
	Mesh_Standard sphere;
	Transform skyboxTransform;

	D3D11_MAPPED_SUBRESOURCE skyboxMappedResource;
	ID3D11SamplerState* skyboxSampler;

	ID3D11VertexShader* skyVertexShader;
	ID3D11PixelShader* skyPixelShader;
	ID3D11InputLayout* skyLayout;


	
};


#endif
