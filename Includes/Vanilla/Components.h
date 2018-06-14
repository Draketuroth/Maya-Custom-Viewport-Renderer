#ifndef COMPONENTS_H
#define COMPONENTS_H

#include "VertexTypes.h"
#include "Datatypes.h"
#include "DebugHelper.h"

#include "Mesh_Standard.h"
#include "Transform.h"
#include "Material.h"

class Components {

public:

	Components();
	~Components();

	void Shutdown();

	void SetStaticMesh(Mesh_Standard &mesh);
	Mesh_Standard& GetStaticMesh() { return staticMesh; };

	void SetTransform(Transform &transform);
	Transform& GetTransform() { return transform; };

	void SetMaterial(Material* material);
	Material* GetMaterial() { return material; };

private:

	Transform transform;
	Mesh_Standard staticMesh;
	Material* material;
	
};


#endif

