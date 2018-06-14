
#include "ApplicationCore.h"

ApplicationCore* ApplicationCore::coreInstance = nullptr;

ApplicationCore::ApplicationCore() {

	window = nullptr;
	deviceResources = nullptr;
	renderEngine = nullptr;
	materialManager = nullptr;
	meshManager = nullptr;
	timer = nullptr;
	transformManager = nullptr;
	camera = nullptr;

}

ApplicationCore::~ApplicationCore() {


}

void ApplicationCore::Initialize() {

	HRESULT hr = S_OK;

	// Create the window
	window = new WindowManager();

	// Create the DirectX Core components
	deviceResources = new DeviceManager();
	deviceResources->Initialize(window->getWindowHandle());

	// Initialize the global timer
	timer = new Timer();

	meshManager = new MeshManager();

	materialManager = new MaterialManager();

	transformManager = new TransformManager();

	lightManager = new LightManager();
	lightManager->Initialize();

	//Create the engine renderer and send a reference of the Device Manager
	renderEngine = new Renderer();
	renderEngine->Initialize(deviceResources->GetDevice(), deviceResources->GetDeviceContext());

	// Gain access to the shared memory. Set circular buffer size to 100 megabytes. 
	size_t buffSize = 100 * 1 << 20;
	comLib = new ComLib("MyFileMappingObject", buffSize, TYPE::CONSUMER);

	// Initialize temporary camera
	camera = new Camera();

	hr = CreateConstantBuffer(deviceResources->GetDevice());

	if (FAILED(hr)) {

		MessageBox(
			NULL,
			L"CRITICAL ERROR: Default constant buffer could not be created\nClosing application...",
			L"ERROR",
			MB_OK);

		PostQuitMessage(0);
	}

	// Application will now run until the user presses escape or quits by closing the window
	Run();

}

void ApplicationCore::Shutdown() {

	ApplicationCore::GetInstance()->GetDevices()->GetDeviceContext()->ClearState();

	// Are there any entities in the scene?
	if (entities.size() > 0) {

		for (auto& iterator : entities) {

			entities.at(iterator.first).Shutdown();

		}

	}

	coreInstance->renderEngine->Shutdown();
	SAFE_DELETE(coreInstance->renderEngine);

	coreInstance->window->Shutdown();
	SAFE_DELETE(coreInstance->window);

	SAFE_DELETE(coreInstance->timer);

	coreInstance->meshManager->Shutdown();
	SAFE_DELETE(coreInstance->meshManager);

	coreInstance->materialManager->Shutdown();
	SAFE_DELETE(coreInstance->materialManager);

	coreInstance->transformManager->Shutdown();
	SAFE_DELETE(coreInstance->transformManager);

	coreInstance->lightManager->Shutdown();
	SAFE_DELETE(coreInstance->lightManager);

	// Unmap the shared memory and delete ComLib subsystem
	coreInstance->comLib->Unmap();
	SAFE_DELETE(coreInstance->comLib);

	SAFE_DELETE(coreInstance->camera);

	SAFE_RELEASE(gConstantBuffer);

	coreInstance->deviceResources->Shutdown();
	SAFE_DELETE(coreInstance->deviceResources);

	// Finally, remove the application core instance
	delete coreInstance;
	coreInstance = nullptr;

}

void ApplicationCore::CreateEntity(string name) {

	// Insert or assign to the already existing entity
	entities.insert_or_assign(name, Entity());
}

void ApplicationCore::RemoveEntity(string name) {

	if (FindEntity(name)){

		entities.at(name).Shutdown();
		entities.erase(name);

	}

	else {

		cout << "Entity could not be deleted. Entity " + name + " could not be found";
	}
}

Entity& ApplicationCore::GetEntity(string name) {

	return entities.at(name);
}

void ApplicationCore::UpdateEntityName(string oldName, string newName) {

	auto found = entities.find(oldName);
	if (found != entities.end()) {
		
		// Make a backup of the entity
		Entity& backup = entities.at(oldName);

		// Add it again with new key
		entities.insert({ newName, backup });

		// Remove the entity 
		entities.erase(oldName);
	}
}

bool ApplicationCore::FindEntity(string name) {

	auto found = entities.find(name);
	if (found != entities.end()) {
		
		return true;
	}

	else {
	
		return false;
	}
}

void ApplicationCore::Update() {


}

void ApplicationCore::CreateInstance() {

	if (!coreInstance) {

		coreInstance = new ApplicationCore();
	}
}

ApplicationCore* ApplicationCore::GetInstance() {

	return coreInstance;
}

HRESULT ApplicationCore::CreateConstantBuffer(ID3D11Device* gDevice) {

	HRESULT hr = S_OK;

	CONSTANT_BUFFER ConstData;

	// World Matrix
	XMStoreFloat4x4(&ConstData.matrixWorld, XMMatrixIdentity());
	XMStoreFloat4x4(&ConstData.matrixWorldInvTranspose, XMMatrixIdentity());
	XMStoreFloat4x4(&ConstData.matrixView, XMMatrixIdentity());
	XMStoreFloat4x4(&ConstData.matrixProjection, XMMatrixIdentity());

	// Constant buffer description
	D3D11_BUFFER_DESC constBufferDesc;
	ZeroMemory(&constBufferDesc, sizeof(constBufferDesc));
	constBufferDesc.ByteWidth = sizeof(CONSTANT_BUFFER);
	constBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	constBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	constBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	constBufferDesc.MiscFlags = 0;
	constBufferDesc.StructureByteStride = 0;

	// Subresource data
	D3D11_SUBRESOURCE_DATA constData;
	constData.pSysMem = &ConstData;
	constData.SysMemPitch = 0;
	constData.SysMemSlicePitch = 0;

	// Create buffer
	hr = gDevice->CreateBuffer(&constBufferDesc, &constData, &gConstantBuffer);

	if (FAILED(hr)) {

		return S_FALSE;
	}

	setDebugName(gConstantBuffer, "STANDARD_CONSTANT_BUFFER");

	return hr;

}

int ApplicationCore::Run() {

	// If window isn't visible at this point, make it visible
	if (!IsWindowVisible(window->getWindowHandle())) {

		ShowWindow(window->getWindowHandle(), SW_SHOW);
	}

	MSG windowMessage;
	windowMessage.message = WM_NULL;

	// Initialize the timer
	timer->initialize();

	// Comlib temporary variables
	size_t delay = 0;
	size_t msgSize = 200;
	char* msg = new char[msgSize];

	while (windowMessage.message != WM_QUIT) {

		if (PeekMessage(&windowMessage, NULL, NULL, NULL, PM_REMOVE)) {

			TranslateMessage(&windowMessage);
			DispatchMessage(&windowMessage);
		}
		
		// If there are no messages to handle, the application will continue by running a frame
		else {

			// At the start of every iteration, gather the delta time
			float deltaTime = timer->getDeltaTime();

			// Show the fps
			//window->showFPS(deltaTime);

			// If a delay greater than 0 is specified, we delay the application
			if (delay != 0) {
				Sleep(delay);
			}

			// Get the next message length from Maya
			size_t nextLength = comLib->nextSize();

			// If the next message size is greater than the message size, we must allocate memory for a larger message
			if (nextLength > msgSize) {

				// Delete the allocated message
				delete msg;

				// Allocate memory for larger message
				msg = new char[nextLength];

				// Set the new message size
				msgSize = nextLength;
			}
			
			// Listen for messages
			if (comLib->recv(msg, nextLength)) {
				
				MsgHeader* header = (MsgHeader*)msg;

				switch (header->type) {
					
				case MAYA_MSG_TYPE::NEW_MESH:
					
				{
					
					/////////////////////////////////////////////////////////////
					// MESH
					/////////////////////////////////////////////////////////////
					
					// Get mesh data such as name and vertices
					MsgMesh* msgMesh = (MsgMesh*)(header + 1);
					int vertexCount = msgMesh->vertexCount;
					
					// Create entity given the mesh name
					CreateEntity(msgMesh->name);

					/////////////////////////////////////////////////////////////
					// MATERIAL
					/////////////////////////////////////////////////////////////

					// After the mesh message comes the material
					MsgMaterial* msgMaterial = (MsgMaterial*)(msgMesh + 1);

					// Create the material
					coreInstance->GetMaterialManager()->CreateMaterial(msgMaterial[0]);

					/////////////////////////////////////////////////////////////
					// VERTICES
					/////////////////////////////////////////////////////////////

					// After the mesh data comes the vertices, so receive them and create the vertex buffer
					Vertex* v = (Vertex*)(msgMaterial + 1);

					coreInstance->GetMeshManager()->CreateMesh(msgMesh->name, v, vertexCount);

					/////////////////////////////////////////////////////////////
					// TRANSFORM
					/////////////////////////////////////////////////////////////
					
					// After the vertices comes the transform of the new mesh
					MsgTransform* msgTransform = (MsgTransform*)(v + vertexCount);
					
					XMMATRIX transformMatrix = XMLoadFloat4x4(&msgTransform->transform);

					coreInstance->GetTransformManager()->CreateTransformBuffer(
						msgTransform->childName,
						transformMatrix);

					/*coreInstance->GetTransformManager()->CreateTransformBuffer(
						msgTransform->childName,
						msgTransform->translation,
						msgTransform->quaternion,
						msgTransform->scale);*/
				}
				break;

				case MAYA_MSG_TYPE::MESH_REMOVED:
				{
					MsgMesh* msgMesh = (MsgMesh*)(header + 1);

					coreInstance->RemoveEntity(msgMesh->name);
					
				}
				break;
				
				case MAYA_MSG_TYPE::MESH_RENAMED:
				{
					MsgMesh* msgMesh = (MsgMesh*)(header + 1);

					coreInstance->UpdateEntityName(msgMesh->oldName, msgMesh->name);
				}
					break;

				case MAYA_MSG_TYPE::MESH_TOPOLOGY_CHANGED:
				{
					// Get mesh data such as name and vertices
					MsgMesh* msgMesh = (MsgMesh*)(header + 1);
					int vertexCount = msgMesh->vertexCount;

					// After the mesh data comes the vertices, so receive them and create the vertex buffer
					Vertex* v = (Vertex*)(msgMesh + 1);

					coreInstance->GetMeshManager()->RebuildMesh(msgMesh->name, v, vertexCount);
				}
				break;
				
				case MAYA_MSG_TYPE::MESH_TRANSFORM_CHANGED:
				{
					MsgTransform* msgTransform = (MsgTransform*)(header + 1);

					XMMATRIX transformMatrix = XMLoadFloat4x4(&msgTransform->transform);
					
					// Update the transform buffer
					coreInstance->GetTransformManager()->UpdateTransformBuffer(
						msgTransform->childName,
						transformMatrix);

					/*coreInstance->GetTransformManager()->UpdateTransformBuffer(
						msgTransform->childName,
						msgTransform->translation,
						msgTransform->quaternion,
						msgTransform->scale);*/

				}
				break;

				case MAYA_MSG_TYPE::MATERIAL_CHANGED:
				{
					MsgMaterial* msgMaterial = (MsgMaterial*)(header + 1);

					// Update the given material for all meshes currently assigned to it
					coreInstance->GetMaterialManager()->UpdateMaterialBuffer(
						msgMaterial->materialName,
						msgMaterial->diffuse);

				}
				break;

				case MAYA_MSG_TYPE::MATERIAL_RENAMED:
				{
					MsgMaterial* msgMaterial = (MsgMaterial*)(header + 1);

					coreInstance->GetMaterialManager()->UpdateMaterialName(msgMaterial->oldName, msgMaterial->name);
				}
				break;
				
				case MAYA_MSG_TYPE::VIEW_CHANGED:
				{
					MsgCamera* msgCamera = (MsgCamera*)(header + 1);
					
					// Load the matrices to XMMATRIX
					XMMATRIX view = XMLoadFloat4x4(&msgCamera->viewMatrix);

					// Invert from right handed coordinate system to left coordinate system
					view = view * XMMatrixScaling(1, 1, -1);

					// Create the final matrices
					XMFLOAT4X4 finalView;;

					// Store them
					XMStoreFloat4x4(&finalView, view);
					
					// Set them to the camera
					camera->SetPosition(msgCamera->position.x, msgCamera->position.y, msgCamera->position.z);
					camera->mView = finalView;
					
					// Is the camera ortographic or not?
					if (msgCamera->isOrtographic == 1) {

						camera->SetOrthographicProjection(msgCamera->viewWidth, msgCamera->fPlane, msgCamera->nPlane);
					}

					else {

						camera->SetPerspectiveProjection(msgCamera->fov, msgCamera->fPlane, msgCamera->nPlane);
					}

					// Update the camera buffer
					camera->UpdateBuffer();
				}
				
				break;
				
				case MAYA_MSG_TYPE::NEW_LIGHT:
				{
					MsgLight* msgLight = (MsgLight*)(header + 1);

					coreInstance->GetLightManager()->CreatePointLight(msgLight[0]);
					coreInstance->GetLightManager()->UpdatePointLightBuffer();
				}
				break;
				
				case MAYA_MSG_TYPE::LIGHT_REMOVED:
				{
					MsgLight* msgLight = (MsgLight*)(header + 1);

					coreInstance->GetLightManager()->RemovePointLight(msgLight->name);
				}
				break;
				
				case MAYA_MSG_TYPE::LIGHT_RENAMED:
				{
					MsgLight* msgLight = (MsgLight*)(header + 1);

					coreInstance->GetLightManager()->UpdateLightName(msgLight->oldName, msgLight->name);

				}
				break;
				
				case MAYA_MSG_TYPE::LIGHT_TRANSFORM_CHANGED:
				{
					MsgLight* msgLight = (MsgLight*)(header + 1);

					coreInstance->GetLightManager()->UpdatePointLightTranslation(msgLight->name, msgLight->Position);
					coreInstance->GetLightManager()->UpdatePointLightBuffer();
				}
				break;

				case MAYA_MSG_TYPE::LIGHT_COLOR_CHANGED:
				{
					MsgLight* msgLight = (MsgLight*)(header + 1);

					coreInstance->GetLightManager()->UpdatePointLightColor(msgLight->name, msgLight->Color);
					coreInstance->GetLightManager()->UpdatePointLightBuffer();
				}
				break;
				}

			}

			// Clear the G-buffers
			renderEngine->ResetDeferred();

			// DEFERRED RENDER PASS
			// If there are any entities, render each active entity to the G-buffers
			if (entities.size() > 0) {

				for (auto& iterator : entities) {

					if (!renderEngine->RenderStaticObjectToTexture(iterator.first)) {

						cout << "Entity " + iterator.first + " was not found" << endl;
					}

				}

				// LIGHT PASS
				// Initialize light pass
				renderEngine->RenderLightPass();

			}
			
			// Present the rendered image
			deviceResources->GetSwapChain()->Present(0, 0);

			// Update delta time
			timer->updateCurrentTime();

		}

	}

	delete msg;

	return 0;
}