#ifndef MESHMANAGER_H
#define MESHMANAGER_H

#include "Datatypes.h"
#include "VertexTypes.h"
#include "DebugHelper.h"

#include "Mesh_Standard.h"

class MeshManager
{

public:

	MeshManager();
	~MeshManager();

	void CreateMesh(string entityID, Vertex* vertices, unsigned int vertexCount);
	void RebuildMesh(string entityID, Vertex* vertices, unsigned int vertexCount);

	Mesh_Standard GenerateCube(float width, float height, float depth);
	Mesh_Standard GenerateSphere(float radius, UINT sliceCount, UINT stackCount);

	void Shutdown();

private:



};

#endif
