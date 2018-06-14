#ifndef WINDOWMANAGER_H
#define WINDOWMANAGER_H

#include <d3d11.h>	
#include <d3dcompiler.h>	
#include <DirectXMath.h>

#include "MacroDefinitions.h"
#include <iostream>
#include <sstream>

using namespace DirectX;
using namespace std;

class WindowManager {

public:

	WindowManager(int width = 1920, int height = 1080);
	~WindowManager();

	void Initialize();
	
	//----------------------------------------------------------------------------------------------------------------------------------//
	// WINDOW FUNCTIONS
	//----------------------------------------------------------------------------------------------------------------------------------//
	HRESULT CreateApplicationWindow();
	void Shutdown();

	HWND getWindowHandle() { return windowHandle; };
	int GetWindowWidth() { return windowWidth; };
	int GetWindowHeight() { return windowHeight; };

	static LRESULT CALLBACK WindowProcedure(HWND windowHandle, UINT message, WPARAM wParam, LPARAM lParam);

	void showFPS(float deltaTime);

	//----------------------------------------------------------------------------------------------------------------------------------//
	// CURSOR FUNCTIONS
	//----------------------------------------------------------------------------------------------------------------------------------//



	void GetMaskBitmaps(HBITMAP hSourceBitmap, 
						COLORREF clrTransparent,
						HBITMAP &hAndMaskBitmap, 
						HBITMAP &hXorMaskBitmap);

	HCURSOR CreateCursorFromBitmap(HBITMAP hSourceBitmap,
								   COLORREF clrTransparent,
								   DWORD xHotspot,
								   DWORD   yHotspot);

	HICON CreateIcon(const wchar_t* filename);
	
private:

	int windowWidth;
	int windowHeight;

	HWND windowHandle;
	RECT w_RC;

};

static HINSTANCE w_hInstance;
static wstring w_className;

#endif