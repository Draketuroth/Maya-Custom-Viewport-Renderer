/////////////////////////////////////////////////////////////
// UD1414 Project: Maya Viewport to custom engine
// Fredrik Linde TA15
/////////////////////////////////////////////////////////////


#ifndef APPLICATIONCORE_H
#define APPLICATIONCORE_H

#include "WindowManager.h"
#include "DeviceManager.h"
#include "Renderer.h"
#include "Timer.h"
#include "Entity.h"
#include "Camera.h"
#include "MeshManager.h"
#include "MaterialManager.h"
#include "TransformManager.h"
#include "LightManager.h"
#include <unordered_map>

// Comlib library
#include <Shared\Comlib.h>
#pragma comment(lib, "Shared.lib")

class ApplicationCore {

public:

	static void CreateInstance();
	static ApplicationCore* GetInstance();

	void Initialize();
	void Shutdown();

	void CreateEntity(string name);
	void RemoveEntity(string name);

	bool FindEntity(string name);
	Entity& GetEntity(string name);
	void UpdateEntityName(string oldName, string newName);

	void Update();
	
	WindowManager*		GetWindow() const { return window; };
	DeviceManager*		GetDevices() const { return deviceResources; };
	Renderer*			GetRenderer() const { return renderEngine; };
	Timer*				GetTimer() const { return timer; };
	MeshManager*		GetMeshManager() const { return meshManager; };
	MaterialManager*	GetMaterialManager() const { return materialManager; };
	TransformManager*	GetTransformManager() const { return transformManager; };
	LightManager*		GetLightManager() const { return lightManager; };

	Camera*				GetCamera() const { return camera; };

	ID3D11Buffer*		GetConstantBuffer()  { return gConstantBuffer; };
	ID3D11Buffer*		const* GetConstBuffer() const { return &gConstantBuffer; };


private:

	ApplicationCore();
	~ApplicationCore();

	int Run();

	static ApplicationCore* coreInstance;

	WindowManager*		window;
	DeviceManager*		deviceResources;
	Renderer*			renderEngine;
	Timer*				timer;
	MeshManager*		meshManager;
	MaterialManager*	materialManager;
	TransformManager*	transformManager;
	LightManager*		lightManager;
	ComLib*				comLib;

	Camera* camera;

	ID3D11Buffer* gConstantBuffer;
	HRESULT CreateConstantBuffer(ID3D11Device* gDevice);

	unordered_map<string, Entity>entities;

};



#endif
