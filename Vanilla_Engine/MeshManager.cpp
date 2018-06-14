
#include "MeshManager.h"

#include "ApplicationCore.h"

MeshManager::MeshManager() {

	
}

MeshManager::~MeshManager() {


}

void MeshManager::Shutdown() {

	
}

void MeshManager::CreateMesh(string entityID, Vertex* vertices, unsigned int vertexCount) {

	// Find the entity
	if (ApplicationCore::GetInstance()->FindEntity(entityID)) {

		Mesh_Standard mesh;

		vector<Vertex>vert;
		vert.resize(vertexCount);

		for (int i = 0; i < vertexCount; i++) {

			vert[i] = vertices[i];
		}

		mesh.SetVertices(vert);
		mesh.vertexBuffer = ApplicationCore::GetInstance()->GetDevices()->CreateStaticVertexBuffer(vertices, vertexCount);

		// Set the skinned mesh for this entity
		ApplicationCore::GetInstance()->GetEntity(entityID).GetComponents().SetStaticMesh(mesh);

	}

	else {

		cout << "Mesh could not be created. Entity " + entityID + " was not found!" << endl;
	}
}

void MeshManager::RebuildMesh(string entityID, Vertex* vertices, unsigned int vertexCount) {

	// Find the entity
	if(ApplicationCore::GetInstance()->FindEntity(entityID)){

		// Get the mesh
		Mesh_Standard& mesh = ApplicationCore::GetInstance()->GetEntity(entityID).GetComponents().GetStaticMesh();

		// Release the vertex buffer
		SAFE_RELEASE(mesh.vertexBuffer);

		vector<Vertex>vert;
		vert.resize(vertexCount);

		for (int i = 0; i < vertexCount; i++) {

			vert[i] = vertices[i];
		}

		mesh.SetVertices(vert);
		mesh.vertexBuffer = ApplicationCore::GetInstance()->GetDevices()->CreateStaticVertexBuffer(vertices, vertexCount);

	}

	else {

		cout << "Mesh could not be rebuild. Entity " + entityID + " was not found!" << endl;
	}
}

Mesh_Standard MeshManager::GenerateCube(float width, float height, float depth) {

	Mesh_Standard mesh;

	// First create the vertices
	Vertex vertex[24];

	float w = 0.5f * width;
	float h = 0.5f * height;
	float d = 0.5f * depth;

	// Front face
	// Normal points points out from the scene, -Z

	vertex[0] = Vertex(-w, -h, -d, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f);
	vertex[1] = Vertex(-w, +h, -d, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f);
	vertex[2] = Vertex(+w, +h, -d, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f);
	vertex[3] = Vertex(+w, -h, -d, 0.0f, 0.0f, -1.0f, 1.0f, 1.0f);

	// Back face
	// Normal points into the scene, +Z

	vertex[4] = Vertex(-w, -h, +d, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f);
	vertex[5] = Vertex(+w, -h, +d, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f);
	vertex[6] = Vertex(+w, +h, +d, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f);
	vertex[7] = Vertex(-w, +h, +d, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f);

	// Top face
	// Normal points up in the screen, +Y

	vertex[8] = Vertex(-w, +h, -d, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f);
	vertex[9] = Vertex(-w, +h, +d, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f);
	vertex[10] = Vertex(+w, +h, +d, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f);
	vertex[11] = Vertex(+w, +h, -d, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f);

	// Bottom face
	// Normal points down in the scene, -Y
	vertex[12] = Vertex(-w, -h, -d, 0.0f, -1.0f, 0.0f, 1.0f, 1.0f);
	vertex[13] = Vertex(+w, -h, -d, 0.0f, -1.0f, 0.0f, 0.0f, 1.0f);
	vertex[14] = Vertex(+w, -h, +d, 0.0f, -1.0f, 0.0f, 0.0f, 0.0f);
	vertex[15] = Vertex(-w, -h, +d, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f);

	// Left face
	// Normal points left in the scene, -X

	vertex[16] = Vertex(-w, -h, +d, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f);
	vertex[17] = Vertex(-w, +h, +d, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f);
	vertex[18] = Vertex(-w, +h, -d, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f);
	vertex[19] = Vertex(-w, -h, -d, -1.0f, 0.0f, 0.0f, 1.0f, 1.0f);

	// Right face
	// Normal points right in the scene, +X
	vertex[20] = Vertex(+w, -h, -d, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f);
	vertex[21] = Vertex(+w, +h, -d, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f);
	vertex[22] = Vertex(+w, +h, +d, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f);
	vertex[23] = Vertex(+w, -h, +d, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f);

	vector<Vertex> Vertices;
	Vertices.assign(&vertex[0], &vertex[24]);

	mesh.SetVertices(Vertices);
	mesh.vertexBuffer = ApplicationCore::GetInstance()->GetDevices()->CreateStaticVertexBuffer(&Vertices[0], Vertices.size());

	UINT i[36];

	// Front face indices
	i[0] = 0; i[1] = 1; i[2] = 2;
	i[3] = 0; i[4] = 2; i[5] = 3;

	// Back face indices
	i[6] = 4; i[7] = 5; i[8] = 6;
	i[9] = 4; i[10] = 6; i[11] = 7;

	// Top face indices
	i[12] = 8; i[13] = 9; i[14] = 10;
	i[15] = 8; i[16] = 10; i[17] = 11;

	// Bottom face indices
	i[18] = 12; i[19] = 13; i[20] = 14;
	i[21] = 12; i[22] = 14; i[23] = 15;

	// Left face indices
	i[24] = 16; i[25] = 17; i[26] = 18;
	i[27] = 16; i[28] = 18; i[29] = 19;

	// Right face indices
	i[30] = 20; i[31] = 21; i[32] = 22;
	i[33] = 20; i[34] = 22; i[35] = 23;

	vector<UINT>Indices;
	Indices.assign(&i[0], &i[36]);

	mesh.SetIndices(Indices);
	mesh.indexBuffer = ApplicationCore::GetInstance()->GetDevices()->CreateIndexBuffer(&Indices[0], Indices.size());

	return mesh;
}

Mesh_Standard MeshManager::GenerateSphere(float radius, UINT sliceCount, UINT stackCount) {

	Mesh_Standard mesh;

	vector<Vertex>Vertices;
	vector<UINT>Indices;

	// Compute vertices by starting from the top pole and move down each stack
	Vertex topVertex(0.0f, +radius, 0.0f, 0.0f, +1.0f, 0.0f, 0.0f, 0.0f);
	Vertex bottomVertex(0.0f, -radius, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f, 1.0f);

	// Push back the top vertex
	Vertices.push_back(topVertex);

	float phiStep = XM_PI / stackCount;
	float thetaStep = 2.0f*XM_PI / sliceCount;

	// Compute vertices for each stack ring (do not count the poles as rings).
	for (UINT i = 1; i <= stackCount - 1; ++i)
	{
		float phi = i*phiStep;

		// Vertices of ring.
		for (UINT j = 0; j <= sliceCount; ++j)
		{
			float theta = j*thetaStep;

			Vertex v;

			// spherical to cartesian
			v.pos.x = radius*sinf(phi)*cosf(theta);
			v.pos.y = radius*cosf(phi);
			v.pos.z = radius*sinf(phi)*sinf(theta);

			XMVECTOR p = XMLoadFloat3(&v.pos);
			XMStoreFloat3(&v.normal, XMVector3Normalize(p));

			v.uv.x = theta / XM_2PI;
			v.uv.y = phi / XM_PI;

			Vertices.push_back(v);
		}
	}

	Vertices.push_back(bottomVertex);

	//
	// Compute indices for top stack.  The top stack was written first to the vertex buffer
	// and connects the top pole to the first ring.
	//

	for (UINT i = 1; i <= sliceCount; ++i)
	{
		Indices.push_back(0);
		Indices.push_back(i + 1);
		Indices.push_back(i);
	}

	//
	// Compute indices for inner stacks (not connected to poles).
	//

	// Offset the indices to the index of the first vertex in the first ring.
	// This is just skipping the top pole vertex.
	UINT baseIndex = 1;
	UINT ringVertexCount = sliceCount + 1;
	for (UINT i = 0; i < stackCount - 2; ++i)
	{
		for (UINT j = 0; j < sliceCount; ++j)
		{
			Indices.push_back(baseIndex + i*ringVertexCount + j);
			Indices.push_back(baseIndex + i*ringVertexCount + j + 1);
			Indices.push_back(baseIndex + (i + 1)*ringVertexCount + j);

			Indices.push_back(baseIndex + (i + 1)*ringVertexCount + j);
			Indices.push_back(baseIndex + i*ringVertexCount + j + 1);
			Indices.push_back(baseIndex + (i + 1)*ringVertexCount + j + 1);
		}
	}

	//
	// Compute indices for bottom stack.  The bottom stack was written last to the vertex buffer
	// and connects the bottom pole to the bottom ring.
	//

	// South pole vertex was added last.
	UINT southPoleIndex = (UINT)Vertices.size() - 1;

	// Offset the indices to the index of the first vertex in the last ring.
	baseIndex = southPoleIndex - ringVertexCount;

	for (UINT i = 0; i < sliceCount; ++i)
	{
		Indices.push_back(southPoleIndex);
		Indices.push_back(baseIndex + i);
		Indices.push_back(baseIndex + i + 1);
	}

	mesh.SetVertices(Vertices);
	mesh.vertexBuffer = ApplicationCore::GetInstance()->GetDevices()->CreateStaticVertexBuffer(&Vertices[0], Vertices.size());

	mesh.SetIndices(Indices);
	mesh.indexBuffer = ApplicationCore::GetInstance()->GetDevices()->CreateIndexBuffer(&Indices[0], Indices.size());

	return mesh;
}
