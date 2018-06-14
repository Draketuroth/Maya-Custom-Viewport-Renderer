#ifndef TEXTURECUBE_H
#define TEXTURECUBE_H

#include "DebugHelper.h"
#include "VertexTypes.h"
#include <iostream>

#include <array>

enum class CubeFace {

	POSITIVE_X,
	NEGATIVE_X,
	POSITIVE_Y,
	NEGATIVE_Y,
	POSITIVE_Z,
	NEGATIVE_Z
};

class TextureCube {

public:

	const array<CubeFace, 6> CUBE_FACES = {

		CubeFace::POSITIVE_X,
		CubeFace::NEGATIVE_X,
		CubeFace::POSITIVE_Y,
		CubeFace::NEGATIVE_Y,
		CubeFace::POSITIVE_Z,
		CubeFace::NEGATIVE_Z
	};

	TextureCube();

	// Create texture cube with specified size
	TextureCube(unsigned int size);

	~TextureCube();

	TextureCube& operator = (const TextureCube &otherCube);

	void SetResolution(unsigned int size);
	void SetTextureFormat(DXGI_FORMAT format);
	void SetSRVFormat(DXGI_FORMAT format);

	unsigned int GetResolution() { return resolution; };
	DXGI_FORMAT GetTextureFormat() { return textureFormat; };
	DXGI_FORMAT GetSRVFormat() { return srvFormat; };

	ID3D11Texture2D* cubeTexture;
	ID3D11ShaderResourceView* SRV;

private:

	DXGI_FORMAT srvFormat;
	DXGI_FORMAT textureFormat;
	unsigned int resolution;

};

#endif
