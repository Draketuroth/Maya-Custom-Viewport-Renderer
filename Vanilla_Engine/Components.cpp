
#include "Components.h"

Components::Components() {

	
}

Components::~Components() {


}

void Components::Shutdown() {

	SAFE_RELEASE(staticMesh.vertexBuffer);
	SAFE_RELEASE(transform.transformBuffer);

}

void Components::SetStaticMesh(Mesh_Standard &mesh) {

	this->staticMesh = mesh;
}

void Components::SetTransform(Transform &transform) {

	this->transform = transform;
}

void Components::SetMaterial(Material* material) {

	this->material = material;
}
