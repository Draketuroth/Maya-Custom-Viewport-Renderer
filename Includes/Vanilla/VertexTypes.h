#ifndef VERTEXTYPES_H
#define VERTEXTYPES_H

#include <d3d11.h>	// We require the d3d11 header for Direct3D functions
#include <d3dcompiler.h>	// We also need the D3DCompiler header to compile shaders

#include <DirectXMath.h>

using namespace std;
using namespace DirectX;

struct Vertex
{
	Vertex() {}
	Vertex(const XMFLOAT3& p, const XMFLOAT3& n, const XMFLOAT2& uv) :
		pos(p), normal(n), uv(uv) {}
	Vertex(
		float px, float py, float pz,
		float nx, float ny, float nz,
		float u, float v)
		: pos(px, py, pz), normal(nx, ny, nz),
		  uv(u, v) {}

	XMFLOAT3 pos;
	XMFLOAT3 normal;
	XMFLOAT2 uv;

};

struct PointVertex {

	XMFLOAT3 pos;
	XMFLOAT4 color;
};

#endif
