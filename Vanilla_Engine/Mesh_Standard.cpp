
#include "Mesh_Standard.h"

Mesh_Standard::Mesh_Standard() : Mesh() {


}

Mesh_Standard::~Mesh_Standard() {


}

Mesh_Standard& Mesh_Standard::operator = (const Mesh_Standard &otherMesh) {

	if (this != &otherMesh) {
	
		this->vertices.empty();
		this->vertices.reserve(otherMesh.vertices.size());

		for (UINT i = 0; i < otherMesh.vertices.size(); i++) {

			this->vertices.push_back(otherMesh.vertices[i]);

		}

		this->indices.empty();
		this->indices.reserve(otherMesh.indices.size());

		for (UINT i = 0; i < otherMesh.indices.size(); i++) {

			this->indices.push_back(otherMesh.indices[i]);

		}

		this->vertexBuffer = otherMesh.vertexBuffer;
		this->indexBuffer = otherMesh.indexBuffer;

		return *this;
	}

	else {

		throw "Fatal Error: Can't assign Class object to itself";
	}
}

void Mesh_Standard::SetVertices(vector<Vertex> vertices) {

	this->vertices = vertices;
}

