#ifndef DEBUGHELPER_H
#define DEBUGHELPER_H

#include <d3d11.h>	// We require the d3d11 header for Direct3D functions
#include <d3dcompiler.h>	// We also need the D3DCompiler header to compile shaders

#include "MacroDefinitions.h"
#include <string>
#include <DirectXMath.h>

using namespace DirectX;
using namespace std;

// Used to report live objects when exiting the application
void reportLiveObjects(ID3D11Device* gDevice);

// Used to set specific names for DirectX componenets for debugging with reportLiveObjects()
void setDebugName(ID3D11DeviceChild* child, const string &name);

#endif
