#include "File.h"

void InitD3D(HWND hWnd, DWORD screenWidth, DWORD screenHeight, Limit * pLimit, DWORD nbElts, Coord * pCoord);    // sets up and initializes Direct3D
void UpdateCBuffer(DWORD screenWidth, DWORD screenHeight, Limit * pLimit);
void RenderFrame(DWORD screenWidth, DWORD screenHeight, DWORD nbElts);     // renders a single frame
void CleanD3D(void);        // closes Direct3D and releases memory
