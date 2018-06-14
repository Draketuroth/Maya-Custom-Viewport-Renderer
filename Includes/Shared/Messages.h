#ifndef MESSAGES_H
#define MESSAGES_H

#include "VertexTypes.h"

// ENUM LIST OF MAYA RELATED MESSAGES
enum MAYA_MSG_TYPE {
	NEW_MESH, MESH_RENAMED, MESH_TRANSFORM_CHANGED, MESH_REMOVED, MESH_TOPOLOGY_CHANGED,
	NEW_LIGHT, LIGHT_REMOVED, LIGHT_RENAMED, LIGHT_TRANSFORM_CHANGED, LIGHT_COLOR_CHANGED,
	VIEW_CHANGED,
	NEW_MATERIAL, MATERIAL_REMOVED, MATERIAL_CHANGED, MATERIAL_RENAMED
	};

// STRUCTS AND ENUMS FOR SPECIFIC MAYA CALLBACKS

struct MsgHeader {

	MAYA_MSG_TYPE type;
};

struct MsgTransform {

	char childName[64];

	XMFLOAT3 translation;
	XMFLOAT4 quaternion;
	XMFLOAT3 scale;
	XMFLOAT4X4 transform;

};

struct MsgMesh {

	char name[64];
	char oldName[64];
	unsigned int vertexCount;

};

struct MsgMaterial {

	char name[64];
	char oldName[64];

	char materialName[64];
	char texturePath[128];
	XMFLOAT4 diffuse;
};

struct MsgCamera {

	XMFLOAT4X4 viewMatrix;
	XMFLOAT4 position;
	uint32_t isOrtographic;
	float fPlane;
	float nPlane;
	float fov;
	float viewWidth;

};

struct MsgLight {

	char name[64];
	char oldName[64];

	XMFLOAT4 Color;
	XMFLOAT4 Position;
};


#endif 

