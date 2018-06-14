#ifndef MESH_STANDARD_H
#define MESH_STANDARD_H

#include "Mesh.h"

class Mesh_Standard : public Mesh{

public:

	Mesh_Standard();
	~Mesh_Standard();

	Mesh_Standard& operator = (const Mesh_Standard &otherMesh);

	void SetVertices(vector<Vertex> vertices);
	vector<Vertex> GetVertices() const { return vertices; };

	vector<Vertex>vertices;

private:

};


#endif
