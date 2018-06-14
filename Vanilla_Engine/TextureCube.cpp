
#include "TextureCube.h"

TextureCube::TextureCube() {


}

// Create texture cube with specified size
TextureCube::TextureCube(unsigned int size) {

	resolution = size;
}

TextureCube::~TextureCube() {


}

TextureCube& TextureCube::operator = (const TextureCube &otherCube) {

	// Avoid mesh to assign itself
	if (this != &otherCube) {

		this->cubeTexture = otherCube.cubeTexture;
		this->SRV = otherCube.SRV;

		this->srvFormat = otherCube.srvFormat;
		this->textureFormat = otherCube.textureFormat;
		this->resolution = otherCube.resolution;

		return *this;
	}

	else {

		throw "Fatal Error: Can't assign Class object to itself";
	}
}

void TextureCube::SetResolution(unsigned int size) {

	resolution = size;
}

void TextureCube::SetTextureFormat(DXGI_FORMAT format) {

	this->textureFormat = format;
}

void TextureCube::SetSRVFormat(DXGI_FORMAT format) {

	this->srvFormat = format;
}