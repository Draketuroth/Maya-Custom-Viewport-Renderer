
#include <Material.h>

Material::Material() {


}

Material::~Material() {


}

void Material::Shutdown() {

	SAFE_RELEASE(materialBuffer);
}

Material& Material::operator = (const Material &otherMaterial) {

	this->materialBuffer = otherMaterial.materialBuffer;
	this->name = otherMaterial.name;
	this->diffuse = otherMaterial.diffuse;

	return *this;
}

void Material::SetMaterialBuffer(ID3D11Buffer* materialBuffer) {

	this->materialBuffer = materialBuffer;
}

void Material::SetDiffuse(XMFLOAT4 diffuse) {

	this->diffuse = diffuse;
}

void Material::SetName(string name) {

	this->name = name;
}

void Material::SetTexture(ID3D11ShaderResourceView* texture) {

	this->texture = texture;
}