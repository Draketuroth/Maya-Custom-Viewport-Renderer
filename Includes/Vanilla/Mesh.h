#ifndef MESH_H
#define MESH_H

#include "VertexTypes.h"
#include "Datatypes.h"
#include "DebugHelper.h"

class Mesh {

public:

	Mesh();
	~Mesh();

	void SetIndices(vector<unsigned int> indices);
	vector<unsigned int> GetIndices() { return indices; };

	ID3D11Buffer* vertexBuffer;

	vector<unsigned int> indices;
	ID3D11Buffer* indexBuffer;
	unsigned indexCount;

private:


};


#endif
