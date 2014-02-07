#include "File.h"

void InitD3D(HWND hWnd, int screenWidth, int screenHeight, Limit * pLimit, DWORD nbElts, Coord * pCoord);    // sets up and initializes Direct3D
void RenderFrame(int screenWidth, int screenHeight, DWORD nbElts);     // renders a single frame
void CleanD3D(void);        // closes Direct3D and releases memory
