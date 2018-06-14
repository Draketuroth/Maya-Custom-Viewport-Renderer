#ifndef DATATYPES_H
#define DATATYPES_H

#include "VertexTypes.h"
#include <vector>

//--------------------------------------------------------//
// LIGHT DATATYPES
//--------------------------------------------------------//

struct Light
{
	XMFLOAT3 position;
	XMFLOAT3 color;
};

//--------------------------------------------------------//
// PER FRAME BUFFER CONTENT
//--------------------------------------------------------//

struct TRANSFORM_BUFFER {

	XMFLOAT4X4 matrixW;
	XMFLOAT4X4 entityInvTranspose;
};

struct CONSTANT_BUFFER {

	XMFLOAT4X4 matrixWorld;
	XMFLOAT4X4 matrixWorldInvTranspose;
	XMFLOAT4X4 matrixView;
	XMFLOAT4X4 matrixProjection;
	XMFLOAT4 cameraPos;

};

struct MATERIAL_BUFFER {

	XMFLOAT4 diffuse;
};


#endif
