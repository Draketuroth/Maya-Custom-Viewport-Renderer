#ifndef MATERIALMANAGER_H
#define MATERIALMANAGER_H

#include "Datatypes.h"
#include "DebugHelper.h"

#include "WICTextureLoader.h"
#include "DDSTextureLoader.h"
#include "TextureCube.h"
#include "Material.h"
#include <unordered_map>
#include <Shared\Messages.h>

class MaterialManager {

public:

	MaterialManager();
	~MaterialManager();

	void Shutdown();

	void CreateMaterial(MsgMaterial& material);

	Material* GetMaterial(string name);
	bool FindMaterial(string name);
	void UpdateMaterialName(string oldName, string newName);

	ID3D11ShaderResourceView* CreateTexture(const wchar_t* filename, const int maxSize);

	void CreateMaterialBuffer(string materialName, XMFLOAT4 diffuse);
	void UpdateMaterialBuffer(string materialName, XMFLOAT4 diffuse);

	void LoadTexture(string materialName, const wchar_t* filename, const int maxSize);

	ID3D11ShaderResourceView* CreateDDSTexture(const wchar_t* filename, const int maxSize);
	ID3D11SamplerState* CreateTextureSampler(D3D11_FILTER filter);

	D3D11_SAMPLER_DESC sampDesc;

	ID3D11ShaderResourceView* GetDummyTexture() { return dummyTexture; };

private:

	D3D11_MAPPED_SUBRESOURCE entityMappedResource;

	unordered_map<string, Material*>materials;

	ID3D11ShaderResourceView* dummyTexture;
	vector<ID3D11ShaderResourceView*>textures;
	vector<TextureCube> textureCubes;
	vector<ID3D11SamplerState*>samplerStates;

};

#endif
