
#include "MaterialManager.h"

#include "ApplicationCore.h"

MaterialManager::MaterialManager() {

	textureCubes.reserve(10);
	samplerStates.reserve(10);

	dummyTexture = CreateTexture(L"Textures//BlackPink.jpg", 512);
}

MaterialManager::~MaterialManager() {


}

void MaterialManager::Shutdown() {

	// Are there any materials in the scene?
	if (materials.size() > 0) {

		for (auto& iterator : materials) {

			// Shutdown the material
			materials.at(iterator.first)->Shutdown();

			SAFE_DELETE(iterator.second);

		}

	}

	for (UINT i = 0; i < textures.size(); i++) {

		SAFE_RELEASE(textures[i]);
	}

	for (UINT i = 0; i < textureCubes.size(); i++) {

		SAFE_RELEASE(textureCubes[i].cubeTexture);
		SAFE_RELEASE(textureCubes[i].SRV);

	}

	for (UINT i = 0; i < samplerStates.size(); i++) {

		SAFE_RELEASE(samplerStates[i]);
	}

	//SAFE_RELEASE(dummyTexture);

}

void MaterialManager::CreateMaterial(MsgMaterial& material) {

	string entityName = material.name;
	string materialName = material.materialName;

	// Check if the material already exists or not
	if(materials.count(material.materialName) == 0){

		// Insert the new material
		materials.insert({ material.materialName, new Material() });
		
		// Create the material buffer
		CreateMaterialBuffer(material.materialName, material.diffuse);

		// Convert path to wchar_t
		const size_t cSize = strlen(material.texturePath) + 1;
		wchar_t* wideCharPath = new wchar_t[cSize];
		mbstowcs(wideCharPath, material.texturePath, cSize);

		// Load the texture from the texture path. In case the path is invalid, a dummy texture is given to the material instead
		LoadTexture(material.materialName, wideCharPath, 512);

		delete wideCharPath;

		if(ApplicationCore::GetInstance()->FindEntity(material.name)){

			if (FindMaterial(material.materialName)) {

				ApplicationCore::GetInstance()->GetEntity(material.name).GetComponents().SetMaterial(materials.at(material.materialName));

			}

			else {

				cout << "Material could not be set. Material " + materialName + " was not found" << endl;
			}

		}

		else {

			cout << "Material could not be set. Entity " + entityName + " was not found" << endl;
		}

	}

	else {

		if (ApplicationCore::GetInstance()->FindEntity(material.name)) {

			// Set this entity material to point to the belonging material in the map if it already exists

			if(FindMaterial(material.materialName)){

				ApplicationCore::GetInstance()->GetEntity(material.name).GetComponents().SetMaterial(materials.at(material.materialName));

			}

			else {

				cout << "Material could not be set. Material " + materialName + " was not found" << endl;
			}

		}

		else {

			cout << "Material could not be set. Entity " + entityName + " was not found" << endl;
		}

	}
}

Material* MaterialManager::GetMaterial(string name) {

	return materials.at(name);
}

bool MaterialManager::FindMaterial(string name) {

	auto found = materials.find(name);
	if (found != materials.end()) {

		return true;
	}

	else {

		return false;
	}
}

void MaterialManager::UpdateMaterialName(string oldName, string newName) {

	auto found = materials.find(oldName);
	if (found != materials.end()) {

		// Make a backup of the material
		Material* backup = materials.at(oldName);

		// Add it again with new key
		materials.insert({ newName, backup });

		// Remove the old material
		materials.erase(oldName);
	}

}

ID3D11ShaderResourceView* MaterialManager::CreateTexture(const wchar_t* filename, const int maxSize) {

	HRESULT hr = S_OK;

	ID3D11ShaderResourceView* SRV;

	ID3D11Texture2D* texture2D = nullptr;

	CoInitialize(NULL);
	hr = CreateWICTextureFromFile(ApplicationCore::GetInstance()->GetDevices()->GetDevice(), NULL, filename, NULL, &SRV, maxSize);

	if (FAILED(hr)) {

		MessageBox(
			NULL,
			L"CRITICAL ERROR: Texture could not be loaded\nClosing application...",
			L"ERROR",
			MB_OK);

		PostQuitMessage(0);
	}

	return SRV;
}

void MaterialManager::CreateMaterialBuffer(string materialName, XMFLOAT4 diffuse) {

		HRESULT hr = S_OK;

		// Define the material struct
		MATERIAL_BUFFER materialData;
		materialData.diffuse = diffuse;

		D3D11_BUFFER_DESC bufferDesc;
		ZeroMemory(&bufferDesc, sizeof(bufferDesc));
		bufferDesc.ByteWidth = sizeof(MATERIAL_BUFFER);
		bufferDesc.Usage = D3D11_USAGE_DYNAMIC;
		bufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		bufferDesc.MiscFlags = 0;
		bufferDesc.StructureByteStride = 0;

		D3D11_SUBRESOURCE_DATA constData;
		constData.pSysMem = &materialData;
		constData.SysMemPitch = 0;
		constData.SysMemSlicePitch = 0;

		ID3D11Buffer* temporaryBuffer = nullptr;
		hr = ApplicationCore::GetInstance()->GetDevices()->GetDevice()->CreateBuffer(&bufferDesc, &constData, &temporaryBuffer);

		if (FAILED(hr)) {

			MessageBox(
				NULL,
				L"CRITICAL ERROR: Material buffer could not be created\nClosing application...",
				L"ERROR",
				MB_OK);

			PostQuitMessage(0);

		}

		// Set debug name
		setDebugName(temporaryBuffer, "MATERIAL_BUFFER_ENTITY_ID: " + materialName);

		// Set the buffer for the material
		materials.at(materialName)->SetMaterialBuffer(temporaryBuffer);
		materials.at(materialName)->SetDiffuse(diffuse);
		materials.at(materialName)->SetName(materialName);

}

void MaterialManager::UpdateMaterialBuffer(string materialName, XMFLOAT4 diffuse) {

	if(FindMaterial(materialName)){

		// Get entity material reference
		Material* material = materials.at(materialName);

		ZeroMemory(&entityMappedResource, sizeof(entityMappedResource));
		ApplicationCore::GetInstance()->GetDevices()->GetDeviceContext()->Map(material->materialBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &entityMappedResource);
		MATERIAL_BUFFER* bufferPointer = (MATERIAL_BUFFER*)entityMappedResource.pData;

		bufferPointer->diffuse = diffuse;

		ApplicationCore::GetInstance()->GetDevices()->GetDeviceContext()->Unmap(material->materialBuffer, 0);

	}

	else {

		cout << "Material could not be updated. Material " + materialName + " could not be found!" << endl;
	}
}

void MaterialManager::LoadTexture(string materialName, const wchar_t* filename, const int maxSize) {

	HRESULT hr = S_OK;

	ID3D11ShaderResourceView* texture;

	ID3D11Texture2D* texture2D = nullptr;

	CoInitialize(NULL);
	hr = CreateWICTextureFromFile(ApplicationCore::GetInstance()->GetDevices()->GetDevice(), NULL, filename, NULL, &texture, maxSize);

	if (FAILED(hr)) {

		// Set a dummy texture in case no texture is connected to the entity
		materials.at(materialName)->SetTexture(dummyTexture);
	}

	else {
	
		textures.push_back(texture);
		materials.at(materialName)->SetTexture(texture);

	}
}

ID3D11ShaderResourceView* MaterialManager::CreateDDSTexture(const wchar_t* filename, const int maxSize) {

	HRESULT hr = S_OK;

	ID3D11ShaderResourceView* SRV;

	ID3D11Resource* texture2D = nullptr;

	CoInitialize(NULL);
	DDS_ALPHA_MODE* alphaMod = nullptr;
	hr = CreateDDSTextureFromFile(ApplicationCore::GetInstance()->GetDevices()->GetDevice(), filename, NULL, &SRV, maxSize, alphaMod);

	if (FAILED(hr)) {

		MessageBox(
			NULL,
			L"CRITICAL ERROR: DDS Texture could not be loaded\nClosing application...",
			L"ERROR",
			MB_OK);

		PostQuitMessage(0);
	}

	return SRV;

}

ID3D11SamplerState* MaterialManager::CreateTextureSampler(D3D11_FILTER filter) {

	ID3D11SamplerState* texSampler;

	HRESULT hr = S_OK;

	ZeroMemory(&sampDesc, sizeof(sampDesc));

	sampDesc.Filter = filter;
	sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	sampDesc.MinLOD = 0;
	sampDesc.MaxLOD = D3D11_FLOAT32_MAX;

	hr = ApplicationCore::GetInstance()->GetDevices()->GetDevice()->CreateSamplerState(&sampDesc, &texSampler);

	if (FAILED(hr)) {

		MessageBox(
			NULL,
			L"CRITICAL ERROR: Texture Sampler could not be created\nClosing application...",
			L"ERROR",
			MB_OK);

		PostQuitMessage(0);
	}

	samplerStates.push_back(texSampler);

	return texSampler;
}
