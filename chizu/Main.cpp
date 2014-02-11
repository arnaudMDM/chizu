#include <windows.h>
#include <stdio.h>
#include "File.h"
#include "Draw.h"

// define the screen resolution
#define SCREEN_WIDTH  800
#define SCREEN_HEIGHT 600

// the WindowProc function prototype
LRESULT CALLBACK WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

DWORD nbElts = 0;
BOOL initialized = FALSE;

// the entry point for any Windows program
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow){
	HWND hWnd;
	WNDCLASSEX wc;

	ZeroMemory(&wc, sizeof(WNDCLASSEX));

	wc.cbSize = sizeof(WNDCLASSEX);
	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc = WindowProc;
	wc.hInstance = hInstance;
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.lpszClassName = "WindowClass";

	RegisterClassEx(&wc);

	RECT wr = { 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT };
	AdjustWindowRect(&wr, WS_OVERLAPPEDWINDOW, FALSE);

	hWnd = CreateWindowEx(NULL,
		"WindowClass",
		"Chizu",
		WS_OVERLAPPEDWINDOW,
		300,
		300,
		wr.right - wr.left,
		wr.bottom - wr.top,
		NULL,
		NULL,
		hInstance,
		NULL);

	ShowWindow(hWnd, nCmdShow);

	//Read the file and find the minimum/maximum for x and y; return a list of struct Coord(lat, long)
	Coord* pCoord = NULL;
	Limit limit;
	nbElts = readFile("coord.txt", &pCoord, &limit);
	CHAR strNbElts[200];
	//sprintf_s(strNbElts, "heheeeeeeeeeeeeee %f\n", limit.xMaximum);
	//OutputDebugString(strNbElts);

	// set up and initialize Direct3D
	InitD3D(hWnd, SCREEN_WIDTH, SCREEN_HEIGHT, &limit, nbElts, pCoord);
	initialized = TRUE;
	RenderFrame(SCREEN_WIDTH, SCREEN_HEIGHT, nbElts);

	// enter the main loop:

	MSG msg;

	while (TRUE){
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)){
			TranslateMessage(&msg);
			DispatchMessage(&msg);

			if (msg.message == WM_QUIT){
				break;
			}
		}
	}

	// clean up DirectX and COM
	CleanD3D();

	return msg.wParam;
}

// this is the main message handler for the program
LRESULT CALLBACK WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam){
	switch (message){
		case WM_DESTROY:{
							PostQuitMessage(0);
							return 0;
		} break;
		case WM_WINDOWPOSCHANGED:{
							if (initialized)
								RenderFrame(SCREEN_WIDTH, SCREEN_HEIGHT, nbElts);
							return 0;
		}break;
	}
	return DefWindowProc(hWnd, message, wParam, lParam);
}