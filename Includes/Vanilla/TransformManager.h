#ifndef TRANSFORMMANAGER_H
#define TRANSFORMMANAGER_H

#include "Transform.h"

class TransformManager {

public:

	TransformManager();
	~TransformManager();

	void Shutdown();

	void CreateTransformBuffer(string ID, XMFLOAT3 translation, XMFLOAT4 quaternion, XMFLOAT3 scale);
	void CreateTransformBuffer(string ID, XMMATRIX worldMatrix);

	void UpdateTransformBuffer(string ID, XMFLOAT3 translation, XMFLOAT4 quaternion, XMFLOAT3 scale);
	void UpdateTransformBuffer(string ID, XMMATRIX worldMatrix);


	Transform CreateTransformBuffer();

private:

	D3D11_MAPPED_SUBRESOURCE entityMappedResource;

};


#endif
