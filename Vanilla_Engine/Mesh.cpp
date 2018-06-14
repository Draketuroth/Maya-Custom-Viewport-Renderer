
#include "Mesh.h"

Mesh::Mesh() {

	vertexBuffer = nullptr;

	indexBuffer = nullptr;
	indexCount = -1;
}

Mesh::~Mesh() {


}

void Mesh::SetIndices(vector<unsigned int> indices) {

	this->indices = indices;
}
