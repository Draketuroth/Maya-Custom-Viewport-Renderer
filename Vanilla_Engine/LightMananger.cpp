
#include "LightManager.h"

#include "ApplicationCore.h"

LightManager::LightManager() {

	scalar = 0.0f;
}

LightManager::~LightManager() {


}

void LightManager::Initialize() {

	// Create the pointlight buffer
	CreatePointLightBuffer();
}

void LightManager::Shutdown() {

	SAFE_RELEASE(lightBuffer);
}

bool LightManager::FindLight(string name) {

	auto found = pointlights.find(name);
	if (found != pointlights.end()) {

		return true;
	}

	else {

		return false;
	}
}

void LightManager::UpdateLightName(string oldName, string newName) {

	auto found = pointlights.find(oldName);
	if (found != pointlights.end()) {

		// Make a backup of the light
		Pointlight& backup = pointlights.at(oldName);

		// Add it again with new key
		pointlights.insert({ newName, backup });

		// Remove the old light
		pointlights.erase(oldName);
	}
}

void LightManager::CreatePointLight(MsgLight &light) {

	// Have we reached the max light limit?
	if(pointlights.size() < MAX_LIGHTS){

		// Insert the new light
		pointlights.insert({ light.name, Pointlight(light.Position, light.Color) });

	}

	else {

		cout << "The light limit has been reached. Please remove a light if you wish to add a new one" << endl;
	}
}

void LightManager::RemovePointLight(string name) {

	if(FindLight(name)){

		pointlights.erase(name);
		UpdatePointLightBuffer();

	}

	else {

		cout << "Light could not be removed. Light " + name + " was not found!" << endl;
	}
}

bool LightManager::CreatePointLightBuffer() {

	HRESULT hr = S_OK;

	// Fill the subresource
	LIGHT_BUFFER lBuffer;
	lBuffer.nrOfLights.x = 0;

	for (int i = 0; i < MAX_LIGHTS; i++) {

		lBuffer.Pointlights[i].SetPosition(1.0f, 1.0f, 1.0f, 1.0f);
		lBuffer.Pointlights[i].SetColor(1.0f, 1.0f, 1.0f, 1.0f);
	}

	D3D11_BUFFER_DESC lightBufferDesc;
	ZeroMemory(&lightBufferDesc, sizeof(lightBufferDesc));

	lightBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	lightBufferDesc.ByteWidth = sizeof(LIGHT_BUFFER);
	lightBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	lightBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	lightBufferDesc.MiscFlags = 0;
	lightBufferDesc.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA constData;
	constData.pSysMem = &lBuffer;
	constData.SysMemPitch = 0;
	constData.SysMemSlicePitch = 0;

	ID3D11Buffer* buffer = nullptr;
	hr = ApplicationCore::GetInstance()->GetDevices()->GetDevice()->CreateBuffer(&lightBufferDesc, &constData, &lightBuffer);

	if (FAILED(hr))
	{
		MessageBox(
			NULL,
			L"CRITICAL ERROR: Point light buffer could not be initialized\nClosing application...",
			L"ERROR",
			MB_OK);
		PostQuitMessage(0);
	}

	setDebugName(buffer, "DEFERRED_LIGHT_BUFFER");

	return true;
}

void LightManager::UpdatePointLightTranslation(string name, XMFLOAT4 position) {

	if (FindLight(name)) {

		pointlights.at(name).SetPosition(position);

	}

	else {

		cout << "Light translation could not be updated. Light " + name + " was not found!" << endl;
	}
}

void LightManager::UpdatePointLightColor(string name, XMFLOAT4 color) {

	if (FindLight(name)) {

		pointlights.at(name).SetColor(color);

	}

	else {

		cout << "Light color could not be updated. Light " + name + " was not found!" << endl;
	}
}

void LightManager::UpdatePointLightBuffer() {

	ZeroMemory(&l_mappedResource, sizeof(l_mappedResource));
	ApplicationCore::GetInstance()->GetDevices()->GetDeviceContext()->Map(lightBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &l_mappedResource);
	LIGHT_BUFFER* lightBufferPointer = (LIGHT_BUFFER*)l_mappedResource.pData;

	lightBufferPointer->nrOfLights.x = pointlights.size();

	// Index to keep track of buffer point light indexing
	int index = 0;

	// Create iterator from the pointlights
	for (auto& iterator : pointlights) {

		lightBufferPointer->Pointlights[index] = pointlights[iterator.first];

		// Add to index for each light
		index++;
	}

	ApplicationCore::GetInstance()->GetDevices()->GetDeviceContext()->Unmap(lightBuffer, 0);
}

ID3D11Buffer* LightManager::GetLightBuffer() {

	return lightBuffer;
}