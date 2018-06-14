#ifndef TRANSFORM_H
#define TRANSFORM_H

#include "Datatypes.h"
#include "VertexTypes.h"
#include "DebugHelper.h"

class Transform {

public:

	Transform();
	~Transform();

	Transform& operator = (const Transform &otherTransform);

	void SetTransformBuffer(ID3D11Buffer* transformBuffer);
	void SetTranslation(XMFLOAT3 translation);
	void SetRotation(XMFLOAT3 rotation);
	void SetScaling(XMFLOAT3 scaling);

	XMMATRIX GetTranslationMatrix() { return matrix_translation; };
	XMMATRIX GetRotationMatrix() { return matrix_rotation; };
	XMMATRIX GetScalingMatrix() { return matrix_scaling; };

	XMFLOAT3 GetTranslationVector() { return float3_translation; };
	XMFLOAT3 GetRotationVector() { return float3_rotation; };
	XMFLOAT3 GetScalingVector() { return float3_scaling; };

	ID3D11Buffer* transformBuffer;

private:

	XMMATRIX matrix_translation;
	XMMATRIX matrix_rotation;
	XMMATRIX matrix_scaling;

	XMFLOAT3 float3_translation;
	XMFLOAT3 float3_rotation;
	XMFLOAT3 float3_scaling;
	
};


#endif
