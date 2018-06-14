/////////////////////////////////////////////////////////////
// UD1414 Project: Maya Viewport to custom engine
// Fredrik Linde TA15
// main.cpp
/////////////////////////////////////////////////////////////

#pragma comment (lib, "d3d11.lib")
#pragma comment (lib, "d3dcompiler.lib")
#pragma comment (lib, "dxguid.lib")

#include <DirectXMath.h>
#include "ApplicationCore.h"

using namespace DirectX;
using namespace std;

int main() {
	
	HRESULT hr = S_OK;
	// 516
#ifdef _DEBUG
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);	// Memory leak detection flag
	//_CrtSetBreakAlloc(516);
#endif

	ApplicationCore::CreateInstance();
	ApplicationCore* core = ApplicationCore::GetInstance();

	core->Initialize();
	core->Shutdown();

	return 0;
}

