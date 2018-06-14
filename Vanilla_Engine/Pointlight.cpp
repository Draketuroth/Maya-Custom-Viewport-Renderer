
#include "Pointlight.h"

#include "ApplicationCore.h"

Pointlight::Pointlight() {


}

Pointlight::Pointlight(XMFLOAT4 Position, XMFLOAT4 Color) {

	this->Position = Position;
	this->Color = Color;
}

Pointlight::~Pointlight() {


}

Pointlight& Pointlight::operator = (const Pointlight &otherPointlight) {

	if (this != &otherPointlight) {

		this->Position = otherPointlight.Position;
		this->Color = otherPointlight.Color;

		return *this;
	}

	else {

		throw "Fatal Error: Can't assign Class object to itself";
	}
}

void Pointlight::SetPosition(XMFLOAT4 pos) {

	this->Position = pos;
}

void Pointlight::SetPosition(float x, float y, float z, float w) {

	this->Position.x = x;
	this->Position.y = y;
	this->Position.z = z;
	this->Position.w = w;
}

void Pointlight::SetColor(XMFLOAT4 color) {

	this->Color = color;
}

void Pointlight::SetColor(float r, float g, float b, float a) {

	this->Color.x = r;
	this->Color.y = b;
	this->Color.z = g;
	this->Color.w = a;
}