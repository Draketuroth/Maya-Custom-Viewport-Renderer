#ifndef MATERIAL_H
#define MATERIAL_H

#include "Datatypes.h"
#include "VertexTypes.h"
#include "DebugHelper.h"

class Material {

public:

	Material();
	~Material();
	void Shutdown();

	Material& operator = (const Material &otherMaterial);

	void SetMaterialBuffer(ID3D11Buffer* materialBuffer);
	void SetDiffuse(XMFLOAT4 diffuse);
	void SetName(string name);

	void SetTexture(ID3D11ShaderResourceView* texture);

	XMFLOAT4 GetDiffuse() { return diffuse; };
	string GetName() { return name; };
	ID3D11ShaderResourceView* GetTexture() { return texture; };

	ID3D11Buffer* materialBuffer;

private:

	ID3D11ShaderResourceView* texture;
	string name;
	XMFLOAT4 diffuse;
};


#endif
