
#include "Transform.h"

Transform::Transform() {


}

Transform::~Transform() {


}

Transform& Transform::operator = (const Transform &otherTransform) {

	this->transformBuffer = otherTransform.transformBuffer;

	return *this;
}

void Transform::SetTransformBuffer(ID3D11Buffer* transformBuffer) {

	this->transformBuffer = transformBuffer;
}

void Transform::SetTranslation(XMFLOAT3 translation) {

	matrix_translation = XMMatrixTranslation(translation.x, translation.y, translation.z);
	float3_translation = translation;
}

void Transform::SetRotation(XMFLOAT3 rotation) {

	
}

void Transform::SetScaling(XMFLOAT3 scaling) {

	matrix_scaling = XMMatrixScaling(scaling.x, scaling.y, scaling.z);
	float3_scaling = scaling;
}
