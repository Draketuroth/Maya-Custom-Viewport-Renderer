
#include "WindowManager.h"

WindowManager::WindowManager(int width, int height) {

	w_className = L"Window Class";
	w_hInstance = NULL;

	windowWidth = width;
	windowHeight = height;

	Initialize();
}

WindowManager::~WindowManager() {

	// D0 nothing
}

void WindowManager::Initialize() {

	HRESULT hr = S_OK;

	// Create the window
	hr = CreateApplicationWindow();

	if (FAILED(hr)) {

		MessageBox(
			NULL,
			L"CRITICAL ERROR: Window could not be initialized\nClosing application...",
			L"ERROR",
			MB_OK);

		PostQuitMessage(0);
	}
}

HRESULT WindowManager::CreateApplicationWindow() {

	// HINSTANCE is a handle to an instance. This is the base address of the module in memory and handles the instance of the module to be associated with the window.
	HINSTANCE applicationHandle = GetModuleHandle(NULL);

	// HCURSOR is a handle to a cursor. We can initialize this with built-in or customized cursor bitmaps
	HCURSOR customCursor;

	if (applicationHandle == NULL) {

		// If GetModuleHandle fails, there is no application handle for us to use. The WindowInitialize function returns false and 
		// sends an error message to the console as a string
		cout << "Window Handle Error: Application handle could not be retrieved due to the following error\n"
			<< "Error Code: " << GetLastError() << endl;

		return S_FALSE;

	}

	// Load cursor bitmap
	HBITMAP hSourceBitmap = (HBITMAP)LoadImage(0, L"Textures\\cursor.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);

	if (hSourceBitmap) {

		// Create cursor from the loaded bitmap
		customCursor = CreateCursorFromBitmap(hSourceBitmap, RGB(0, 0, 0), 0, 0);;
	
	}

	else {

		// If the custom cursor bitmap failed to load, present the user with the default cursor. No need to return false, but signal the user about the problem
		cout << "CURSOR ERROR: Custom cursor couldn't be loaded\nPlease check the path or ensure that the bitmap is in the folder\n";

		customCursor = LoadCursor(0, IDC_ARROW);
	}

	HICON icon = CreateIcon(L"Textures\\Logo.ico");

	WNDCLASSEX windowClass = { 0 };	// We start by registering the window class to be used to specify both the behavior and appearence of the window

	windowClass.cbSize = sizeof(WNDCLASSEX);										// The size of the window class in bytes
	windowClass.hInstance = applicationHandle;										// Handle to the instance holding the window procedure
	windowClass.hIcon = icon;														// Load the default window icon
	windowClass.hCursor = customCursor;												// Set the previously loaded custom cursor
	windowClass.style = CS_HREDRAW | CS_VREDRAW;									// We require this to specify this option if height or width is ever changed by the user
	windowClass.hbrBackground = static_cast<HBRUSH>(GetStockObject(BLACK_BRUSH));	// HBRUSH can be used to set the background color of the window
	windowClass.lpfnWndProc = WindowManager::WindowProcedure;						// Pointer to the callback function defined in the "Windows Function Forward Declarations" section
	windowClass.lpszMenuName = NULL;												// The window has no menu 
	windowClass.lpszClassName = L"WindowClass";										// Name for the window class

	if (!RegisterClassEx(&windowClass)) {

		// If registration of the window class fails, then the window can't be created. The WindowInitialize function returns false and 
		// sends an error message to the console as a string
		cout << "Window Class Registration Error: Window Class could not be registered due to the following error\n"
			<< "Error Code: " << GetLastError() << endl;

		return S_FALSE;
	}

	SetCursor(customCursor);
	ShowCursor(TRUE);

	w_RC = { 0, 0, windowWidth, windowHeight };
	//AdjustWindowRect(&w_RC, WS_OVERLAPPEDWINDOW, FALSE);

	windowHandle = CreateWindowEx(

		WS_EX_APPWINDOW,
		L"WindowClass",			// The name of the previously defined window class (Wide string, 16 bit)
		L"Vanilla Engine - Fredrik Linde TA15",		// Text to display at the window title bar (Wide string, 16 bit)
		WS_OVERLAPPEDWINDOW,	//The window style is an overlapped window. An overlapped window has a title bar and a border.
		CW_USEDEFAULT,			// The window's x position in pixels from the monitor's upper left corner
		CW_USEDEFAULT,			// The window's y position in pixels from the monitor's upper left corner
		w_RC.right - w_RC.left,	// Window width in pixels defined in "Global Window Variables"
		w_RC.bottom - w_RC.top,	// Windiw height in pixels defined in "Global Window Variables"
		NULL,					// Here we would normally set the parent, but we only have one window
		NULL,					// Since the window class isn't using a menu, we don¨t have a handle to it
		applicationHandle,		// Handle to the application containing the window 
		this					// Information that we can send to the WM_CREATE message through its lParam member
	);

	if (windowHandle == NULL) {

		cout << "Window Handle Error: Window Handle was not valid and window couldn't be created due to the following error\n"
			<< "Error Code: " << GetLastError() << endl;

		return S_FALSE;
	}

	// The window doesn't show by default, so we must call for it and update it by using the following functions

	SetCapture(windowHandle);
	ShowWindow(windowHandle, SW_SHOWDEFAULT);
	UpdateWindow(windowHandle);

	return true;
}

void WindowManager::Shutdown() {

	DestroyWindow(windowHandle);
}

LRESULT CALLBACK WindowManager::WindowProcedure(HWND windowHandle, UINT message, WPARAM wParam, LPARAM lParam) {

	switch (message) {

	case WM_DESTROY:	// This case only happens when the user presses the window's close button

		PostQuitMessage(0);	// We post a WM_QUIT message with the exit code 0
		break;

	case WM_KEYDOWN:

		if (wParam == VK_ESCAPE) {

			PostQuitMessage(0);
			break;
		}

	default:

		// If a message has not been handled, meaning the window is still open, we sent it to our default window procedure for handling
		return DefWindowProc(windowHandle, message, wParam, lParam);
	}

	return 0;
}

void WindowManager::showFPS(float deltaTime) {

	static int interval;

	int fpsCounter = 1.0f / deltaTime;

	stringstream text_FPS;

	text_FPS << "FPS: " << fpsCounter << endl;

	interval++;

	if (interval == 200) {

		SetWindowTextA(windowHandle, text_FPS.str().c_str());

		interval = 0;
	}
}

void WindowManager::GetMaskBitmaps(HBITMAP hSourceBitmap, COLORREF clrTransparent,
	HBITMAP &hAndMaskBitmap, HBITMAP &hXorMaskBitmap) {

	HDC hDC = ::GetDC(NULL);
	HDC hMainDC = ::CreateCompatibleDC(hDC);
	HDC hAndMaskDC = ::CreateCompatibleDC(hDC);
	HDC hXorMaskDC = ::CreateCompatibleDC(hDC);

	//Get the dimensions of the source bitmap
	BITMAP bm;
	::GetObject(hSourceBitmap, sizeof(BITMAP), &bm);


	hAndMaskBitmap = ::CreateCompatibleBitmap(hDC, bm.bmWidth, bm.bmHeight);
	hXorMaskBitmap = ::CreateCompatibleBitmap(hDC, bm.bmWidth, bm.bmHeight);

	//Select the bitmaps to DC
	HBITMAP hOldMainBitmap = (HBITMAP)::SelectObject(hMainDC, hSourceBitmap);
	HBITMAP hOldAndMaskBitmap = (HBITMAP)::SelectObject(hAndMaskDC, hAndMaskBitmap);
	HBITMAP hOldXorMaskBitmap = (HBITMAP)::SelectObject(hXorMaskDC, hXorMaskBitmap);

	//Scan each pixel of the souce bitmap and create the masks
	COLORREF MainBitPixel;
	for (int x = 0; x<bm.bmWidth; ++x)
	{
		for (int y = 0; y<bm.bmHeight; ++y)
		{
			MainBitPixel = ::GetPixel(hMainDC, x, y);
			if (MainBitPixel == clrTransparent)
			{
				::SetPixel(hAndMaskDC, x, y, RGB(255, 255, 255));
				::SetPixel(hXorMaskDC, x, y, RGB(0, 0, 0));
			}
			else
			{
				::SetPixel(hAndMaskDC, x, y, RGB(0, 0, 0));
				::SetPixel(hXorMaskDC, x, y, MainBitPixel);
			}
		}
	}

	::SelectObject(hMainDC, hOldMainBitmap);
	::SelectObject(hAndMaskDC, hOldAndMaskBitmap);
	::SelectObject(hXorMaskDC, hOldXorMaskBitmap);

	::DeleteDC(hXorMaskDC);
	::DeleteDC(hAndMaskDC);
	::DeleteDC(hMainDC);

	::ReleaseDC(NULL, hDC);
}

HCURSOR WindowManager::CreateCursorFromBitmap(HBITMAP hSourceBitmap,
	COLORREF clrTransparent,
	DWORD   xHotspot, DWORD   yHotspot) {

	HCURSOR hRetCursor = NULL;

	do
	{
		if (NULL == hSourceBitmap)
		{
			break;
		}

		//Create the AND and XOR masks for the bitmap
		HBITMAP hAndMask = NULL;
		HBITMAP hXorMask = NULL;
		GetMaskBitmaps(hSourceBitmap, clrTransparent, hAndMask, hXorMask);
		if (NULL == hAndMask || NULL == hXorMask)
		{
			break;
		}

		//Create the cursor using the masks and the hotspot values provided
		ICONINFO iconinfo = { 0 };
		iconinfo.fIcon = FALSE;
		iconinfo.xHotspot = xHotspot;
		iconinfo.yHotspot = yHotspot;
		iconinfo.hbmMask = hAndMask;
		iconinfo.hbmColor = hXorMask;

		hRetCursor = ::CreateIconIndirect(&iconinfo);

	} while (0);

	return hRetCursor;
}

HICON WindowManager::CreateIcon(const wchar_t* filename) {

	HICON icon = (HICON)LoadImage(NULL,
				filename,
				IMAGE_ICON,
				0,
				0,
				LR_LOADFROMFILE |
				LR_DEFAULTSIZE |
				LR_SHARED);

	return icon;
}

