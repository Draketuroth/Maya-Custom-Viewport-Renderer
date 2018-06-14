#ifndef RENDERER_H
#define RENDERER_H

#include "VertexTypes.h"
#include <string>
#include <memory>
#include <ppltasks.h>

#include "DeferredBuffersClass.h"
#include "DeferredShaderClass.h"
#include "LightShaderClass.h"
#include "Grid.h"
#include "Skybox.h"
#include "TextureCube.h"

class Renderer {

public:

	Renderer();
	~Renderer();

	void Initialize(ID3D11Device* device, ID3D11DeviceContext* deviceContext);
	void Shutdown();

	void Update();
	void RenderLightPass();

	void SetPrimaryRenderTarget();
	bool ResetDeferred();

	void ClearState();
	void ClearShaders();

	void RenderSkybox(ID3D11DeviceContext* gDeviceContext);
	void RenderGrid(ID3D11DeviceContext* gDeviceContext);

	bool RenderStaticObjectToTexture(string ID);

	void SetTextureSampler(ID3D11SamplerState* samplerState);
	ID3D11SamplerState* GetTextureSampler() { return texSampler; };

	DeferredBuffersClass& GetDeferredCore() { return deferredCore; };
	DeferredShaderClass& GetDeferredShaders() { return deferredShaders; };
	LightShaderClass& GetLightShaders() { return lightShaders; };
	GridClass& GetGrid() { return grid; };
	Skybox& GetSkybox() { return skybox; };


private:

	DeferredBuffersClass deferredCore;
	DeferredShaderClass deferredShaders;
	LightShaderClass lightShaders;
	GridClass grid;
	Skybox skybox;

	void CreateShaders(ID3D11Device* gDevice);

	void ResetRenderTarget();
	void Clear();

	ID3D11SamplerState* texSampler;

	ID3D11DeviceContext* deviceContext;
};

#endif
