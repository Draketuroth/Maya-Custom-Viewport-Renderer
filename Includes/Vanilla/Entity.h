#ifndef ENTITY_H
#define ENTITY_H

#include <d3d11.h>	// We require the d3d11 header for Direct3D functions
#include <d3dcompiler.h>	// We also need the D3DCompiler header to compile shaders
#include <DirectXMath.h>
#include <iostream>
#include <vector>
#include "MacroDefinitions.h"

#include "Components.h"

using namespace std;
using namespace DirectX;

class Entity {

public:

	Entity();
	~Entity();
	void Shutdown();

	int id;
	
	Components& GetComponents() { return components; };

private:

	Components components;

	static int GenerateEntityID() {

		static unsigned ID_Number = 0;
		return ID_Number++;
	}

};


#endif
