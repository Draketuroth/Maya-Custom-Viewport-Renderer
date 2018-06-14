#ifndef POINTLIGHT_H
#define POINTLIGHT_H

#include "VertexTypes.h"
#include "Datatypes.h"
#include "DebugHelper.h"

class Pointlight {

public:

	Pointlight();
	Pointlight(XMFLOAT4 Position, XMFLOAT4 Color);
	~Pointlight();

	Pointlight& Pointlight::operator = (const Pointlight &otherPointlight);

	void SetPosition(XMFLOAT4 pos);
	void SetPosition(float x, float y, float z, float w);

	void SetColor(XMFLOAT4 color);
	void SetColor(float r, float g, float b, float a);

	XMFLOAT4 GetPosition() { return Position; };
	XMFLOAT4 GetColor() { return Color; };

private:

	XMFLOAT4 Position;
	XMFLOAT4 Color;
};

#endif