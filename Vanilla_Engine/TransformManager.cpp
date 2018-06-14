
#include "TransformManager.h"

#include "ApplicationCore.h"

TransformManager::TransformManager() {

	
}

TransformManager::~TransformManager() {


}

void TransformManager::Shutdown() {

	
}

void TransformManager::CreateTransformBuffer(string ID, XMFLOAT3 translation, XMFLOAT4 quaternion, XMFLOAT3 scale){

	HRESULT hr = S_OK;

	// Find the entity
	if(ApplicationCore::GetInstance()->FindEntity(ID)){

		// Get the entity transform component
		Transform& entityTransform = ApplicationCore::GetInstance()->GetEntity(ID).GetComponents().GetTransform();

		// Create translation
		XMMATRIX XMTrans = XMMatrixTranslation(translation.x, translation.y, translation.z);

		// Create rotation
		XMVECTOR quat = { quaternion.x, quaternion.y, quaternion.z, quaternion.w };
		XMMATRIX XMRot = XMMatrixRotationQuaternion(quat);

		// Create scale
		XMMATRIX XMScale = XMMatrixScaling(scale.x, scale.y, scale.z);
		
		// Create world matrix and get determinant
		XMMATRIX finalTransform = XMMatrixTranspose(XMScale * XMRot * XMTrans);
		XMVECTOR determinant = XMMatrixDeterminant(finalTransform);
		
		// Define transformation struct
		TRANSFORM_BUFFER transformData;
		XMStoreFloat4x4(&transformData.matrixW, finalTransform);
		XMStoreFloat4x4(&transformData.entityInvTranspose, XMMatrixInverse(&determinant, finalTransform));

		D3D11_BUFFER_DESC bufferDesc;
		ZeroMemory(&bufferDesc, sizeof(bufferDesc));
		bufferDesc.ByteWidth = sizeof(TRANSFORM_BUFFER);
		bufferDesc.Usage = D3D11_USAGE_DYNAMIC;
		bufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		bufferDesc.MiscFlags = 0;
		bufferDesc.StructureByteStride = 0;

		D3D11_SUBRESOURCE_DATA constData;
		constData.pSysMem = &transformData;
		constData.SysMemPitch = 0;
		constData.SysMemSlicePitch = 0;

		ID3D11Buffer* temporaryBuffer = nullptr;
		hr = ApplicationCore::GetInstance()->GetDevices()->GetDevice()->CreateBuffer(&bufferDesc, &constData, &temporaryBuffer);

		if (FAILED(hr)) {

			MessageBox(
				NULL,
				L"CRITICAL ERROR: Transform buffer could not be created\nClosing application...",
				L"ERROR",
				MB_OK);

			PostQuitMessage(0);
		
		}

		setDebugName(temporaryBuffer, "TRANSFORM_BUFFER_ENTITY_ID: " + ID);

		entityTransform.SetTransformBuffer(temporaryBuffer);

	}

	else {

		cout << "Transform buffer could not be created. Entity " + ID + " could not be found" << endl;
	}
}

void TransformManager::CreateTransformBuffer(string ID, XMMATRIX worldMatrix) {

	HRESULT hr = S_OK;

	// Find the entity
	if (ApplicationCore::GetInstance()->FindEntity(ID)) {

		// Get the entity transform component
		Transform& entityTransform = ApplicationCore::GetInstance()->GetEntity(ID).GetComponents().GetTransform();

		XMMATRIX world = XMMatrixTranspose(worldMatrix);
		XMVECTOR determinant = XMMatrixDeterminant(world);

		// Define transformation struct
		TRANSFORM_BUFFER transformData;
		XMStoreFloat4x4(&transformData.matrixW, world);
		XMStoreFloat4x4(&transformData.entityInvTranspose, XMMatrixInverse(&determinant, world));

		D3D11_BUFFER_DESC bufferDesc;
		ZeroMemory(&bufferDesc, sizeof(bufferDesc));
		bufferDesc.ByteWidth = sizeof(TRANSFORM_BUFFER);
		bufferDesc.Usage = D3D11_USAGE_DYNAMIC;
		bufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		bufferDesc.MiscFlags = 0;
		bufferDesc.StructureByteStride = 0;

		D3D11_SUBRESOURCE_DATA constData;
		constData.pSysMem = &transformData;
		constData.SysMemPitch = 0;
		constData.SysMemSlicePitch = 0;

		ID3D11Buffer* temporaryBuffer = nullptr;
		hr = ApplicationCore::GetInstance()->GetDevices()->GetDevice()->CreateBuffer(&bufferDesc, &constData, &temporaryBuffer);

		if (FAILED(hr)) {

			MessageBox(
				NULL,
				L"CRITICAL ERROR: Transform buffer could not be created\nClosing application...",
				L"ERROR",
				MB_OK);

			PostQuitMessage(0);

		}

		setDebugName(temporaryBuffer, "TRANSFORM_BUFFER_ENTITY_ID: " + ID);

		entityTransform.SetTransformBuffer(temporaryBuffer);

	}

	else {

		cout << "Transform buffer could not be created. Entity " + ID + " could not be found" << endl;
	}
}

void TransformManager::UpdateTransformBuffer(string ID, XMFLOAT3 translation, XMFLOAT4 quaternion, XMFLOAT3 scale) {

	// Get the entity
	// Find the entity
	if (ApplicationCore::GetInstance()->FindEntity(ID)) {

		Transform& transform = ApplicationCore::GetInstance()->GetEntity(ID).GetComponents().GetTransform();

		ZeroMemory(&entityMappedResource, sizeof(entityMappedResource));
		ApplicationCore::GetInstance()->GetDevices()->GetDeviceContext()->Map(transform.transformBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &entityMappedResource);
		TRANSFORM_BUFFER* bufferPointer = (TRANSFORM_BUFFER*)entityMappedResource.pData;

		// Create translation
		XMMATRIX XMTrans = XMMatrixTranslation(translation.x, translation.y, translation.z);

		// Create rotation
		XMVECTOR quat = { quaternion.x, quaternion.y, quaternion.z, quaternion.w };
		XMMATRIX XMRot = XMMatrixRotationQuaternion(quat);

		// Create scale
		XMMATRIX XMScale = XMMatrixScaling(scale.x, scale.y, scale.z);

		// Create world matrix and get determinant
		XMMATRIX finalTransform = XMMatrixTranspose(XMScale * XMRot * XMTrans);
		XMVECTOR determinant = XMMatrixDeterminant(finalTransform);

		XMStoreFloat4x4(&bufferPointer->matrixW, finalTransform);
		XMStoreFloat4x4(&bufferPointer->entityInvTranspose, XMMatrixInverse(&determinant, finalTransform));

		ApplicationCore::GetInstance()->GetDevices()->GetDeviceContext()->Unmap(transform.transformBuffer, 0);

	}

	else {

		cout << "Transform could not be updated. Entity " + ID + " could not be found" << endl;
	}
}

void TransformManager::UpdateTransformBuffer(string ID, XMMATRIX worldMatrix) {

	HRESULT hr = S_OK;

	// Find the entity
	if (ApplicationCore::GetInstance()->FindEntity(ID)) {

		Transform& transform = ApplicationCore::GetInstance()->GetEntity(ID).GetComponents().GetTransform();

		ZeroMemory(&entityMappedResource, sizeof(entityMappedResource));
		ApplicationCore::GetInstance()->GetDevices()->GetDeviceContext()->Map(transform.transformBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &entityMappedResource);
		TRANSFORM_BUFFER* bufferPointer = (TRANSFORM_BUFFER*)entityMappedResource.pData;

		XMMATRIX world = XMMatrixTranspose(worldMatrix);
		XMVECTOR determinant = XMMatrixDeterminant(world);

		XMStoreFloat4x4(&bufferPointer->matrixW, world);
		XMStoreFloat4x4(&bufferPointer->entityInvTranspose, XMMatrixInverse(&determinant, world));

		ApplicationCore::GetInstance()->GetDevices()->GetDeviceContext()->Unmap(transform.transformBuffer, 0);
	}

	else {

		cout << "Transform buffer could not be updated. Entity " + ID + " could not be found" << endl;
	}
}

Transform TransformManager::CreateTransformBuffer() {

	HRESULT hr;
	Transform transform;
	TRANSFORM_BUFFER pTransformData;

	XMStoreFloat4x4(&pTransformData.matrixW, XMMatrixIdentity());
	XMStoreFloat4x4(&pTransformData.entityInvTranspose, XMMatrixIdentity());

	D3D11_BUFFER_DESC bufferDesc;
	ZeroMemory(&bufferDesc, sizeof(bufferDesc));
	bufferDesc.ByteWidth = sizeof(TRANSFORM_BUFFER);
	bufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	bufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	bufferDesc.MiscFlags = 0;
	bufferDesc.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA constData;
	constData.pSysMem = &pTransformData;
	constData.SysMemPitch = 0;
	constData.SysMemSlicePitch = 0;

	ID3D11Buffer* temporaryBuffer = nullptr;
	hr = ApplicationCore::GetInstance()->GetDevices()->GetDevice()->CreateBuffer(&bufferDesc, &constData, &temporaryBuffer);

	if (FAILED(hr)) {

		MessageBox(
			NULL,
			L"CRITICAL ERROR: Transform buffer could not be created\nClosing application...",
			L"ERROR",
			MB_OK);

		PostQuitMessage(0);

	}

	transform.SetTransformBuffer(temporaryBuffer);

	return transform;
}