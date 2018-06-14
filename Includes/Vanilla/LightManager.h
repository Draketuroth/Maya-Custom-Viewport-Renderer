#ifndef LIGHTMANAGER_H
#define LIGHTMANAGER_H

#include "VertexTypes.h"
#include "Datatypes.h"
#include "DebugHelper.h"

#include "Pointlight.h"
#include <unordered_map>
#include <Shared\Messages.h>

#define MAX_LIGHTS 10

class LightManager {

private:

	struct LIGHT_BUFFER {

		XMFLOAT4 nrOfLights;
		Pointlight Pointlights[MAX_LIGHTS];
	};

public:

	LightManager();
	~LightManager();

	void Initialize();
	void Shutdown();

	bool FindLight(string name);
	void UpdateLightName(string oldName, string newName);

	// Create a point light
	void CreatePointLight(MsgLight &light);

	// Remove a point light
	void RemovePointLight(string name);

	// Create a light buffer from the given group ID
	bool CreatePointLightBuffer();

	// Update a point light translation
	void UpdatePointLightTranslation(string name, XMFLOAT4 position);

	// Update a point light color
	void UpdatePointLightColor(string name, XMFLOAT4 color);

	// Update the light buffer
	void UpdatePointLightBuffer();

	float scalar;

	// Get the light buffer at the given index
	ID3D11Buffer* GetLightBuffer();


private:

	D3D11_MAPPED_SUBRESOURCE l_mappedResource;

	unordered_map<string, Pointlight>pointlights;
	ID3D11Buffer* lightBuffer;

};

#endif
