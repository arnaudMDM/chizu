// include the basic windows header files and the Direct3D header files
#include <windowsx.h>
#include <d3d11.h>
#include <DirectXMath.h>
#include <D3Dcompiler.h>
#include <WICTextureLoader.h>
#include "Draw.h"

// include the Direct3D Library file
#pragma comment (lib, "d3d11.lib")
#pragma comment (lib, "D3dcompiler.lib")

using namespace DirectX;

// global declarations
IDXGISwapChain *swapchain;             // the pointer to the swap chain interface
ID3D11Device *dev;                     // the pointer to our Direct3D device interface
ID3D11DeviceContext *devcon;           // the pointer to our Direct3D device context
ID3D11RenderTargetView *backbuffer;    // the pointer to our back buffer
ID3D11InputLayout *pLayout;            // the pointer to the input layout
ID3D11VertexShader *pVS;               // the pointer to the vertex shader
ID3D11PixelShader *pPS;                // the pointer to the pixel shader
ID3D11Buffer *pVBuffer;                // the pointer to the vertex buffer
ID3D11Buffer *pCBuffer;                // the pointer to the constant buffer

struct Color{
	FLOAT r, g, b, a;
	Color(FLOAT _r, FLOAT _g, FLOAT _b, FLOAT _a) :r(_r), g(_g), b(_b), a(_a){};
	Color(){};
	FLOAT* getArray(){
		FLOAT res[4] = { r, g, b, a };
		return res;
	}
};

// various buffer structs
struct VERTEX{ FLOAT X, Y, Z, W; Color Normal;};
struct CBUFFER{ FLOAT xCoef, xAff, yCoef, yAff; };

// function prototypes
void InitGraphics(DWORD nbElts, Coord * pCoord);    // creates the shape to render
void InitPipeline(void);    // loads and prepares the shaders
void InitCBuffer(int screenWidth, int screenHeight,Limit * pLimit);

// this function initializes and prepares Direct3D for use
void InitD3D(HWND hWnd, int screenWidth, int screenHeight, Limit * pLimit, DWORD nbElts, Coord * pCoord)
{
	// create a struct to hold information about the swap chain
	DXGI_SWAP_CHAIN_DESC scd;

	// clear out the struct for use
	ZeroMemory(&scd, sizeof(DXGI_SWAP_CHAIN_DESC));

	// fill the swap chain description struct
	scd.BufferCount = 1;                                   // one back buffer
	scd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;    // use 32-bit color
	scd.BufferDesc.Width = screenWidth;                   // set the back buffer width
	scd.BufferDesc.Height = screenHeight;                 // set the back buffer height
	scd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;     // how swap chain is to be used
	scd.OutputWindow = hWnd;                               // the window to be used
	scd.SampleDesc.Count = 4;                              // how many multisamples
	scd.Windowed = TRUE;                                   // windowed/full-screen mode
	scd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;    // allow full-screen switching

	// create a device, device context and swap chain using the information in the scd struct
	D3D11CreateDeviceAndSwapChain(NULL,
		D3D_DRIVER_TYPE_HARDWARE,
		NULL,
		NULL,
		NULL,
		NULL,
		D3D11_SDK_VERSION,
		&scd,
		&swapchain,
		&dev,
		NULL,
		&devcon);

	// get the address of the back buffer
	ID3D11Texture2D *pBackBuffer;
	swapchain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&pBackBuffer);

	// use the back buffer address to create the render target
	dev->CreateRenderTargetView(pBackBuffer, NULL, &backbuffer);
	pBackBuffer->Release();

	// set the render target as the back buffer
	devcon->OMSetRenderTargets(1, &backbuffer, NULL);


	// Set the viewport
	D3D11_VIEWPORT viewport;
	ZeroMemory(&viewport, sizeof(D3D11_VIEWPORT));

	viewport.TopLeftX = 0;    // set the left to 0
	viewport.TopLeftY = 0;    // set the top to 0
	viewport.Width = screenWidth;    // set the width to the window's width
	viewport.Height = screenHeight;    // set the height to the window's height
	viewport.MinDepth = 0;    // the closest an object can be on the depth buffer is 0.0
	viewport.MaxDepth = 1;    // the farthest an object can be on the depth buffer is 1.0

	devcon->RSSetViewports(1, &viewport);

	InitPipeline();
	InitGraphics(nbElts, pCoord);
	InitCBuffer(screenWidth, screenHeight, pLimit);
}


// this is the function used to render a single frame
void RenderFrame(int screenWidth, int screenHeight, DWORD nbElts)
{
	// clear the back buffer to a deep blue
	devcon->ClearRenderTargetView(backbuffer, Color(1.0f, 1.0f, 1.f, 0.0f).getArray());

	// select which vertex buffer to display
	UINT stride = sizeof(VERTEX);
	UINT offset = 0;
	devcon->IASetVertexBuffers(0, 1, &pVBuffer, &stride, &offset);

	// select which primtive type we are using
	devcon->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);

	// draw the vertex buffer to the back buffer
	devcon->Draw(nbElts, 0);

	// switch the back buffer and the front buffer
	swapchain->Present(0, 0);
}


// this is the function that cleans up Direct3D and COM
void CleanD3D(void)
{
	swapchain->SetFullscreenState(FALSE, NULL);    // switch to windowed mode

	// close and release all existing COM objects
	pLayout->Release();
	pVS->Release();
	pPS->Release();
	pVBuffer->Release();
	swapchain->Release();
	backbuffer->Release();
	pCBuffer->Release();
	dev->Release();
	devcon->Release();
}


// this is the function that creates the shape to render
void InitGraphics(DWORD nbElts, Coord * pCoord)
{
	// create vertices to represent the corners of the cube
	VERTEX OurVertices[] =
	{
		{ 0.0f, 0.5f},
		{ 0.45f, -0.5f},
		{ -0.45f, -0.5f}
	};

	// create the vertex buffer
	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));

	bd.Usage = D3D11_USAGE_DYNAMIC;
	bd.ByteWidth = sizeof(Coord)* nbElts;
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

	dev->CreateBuffer(&bd, NULL, &pVBuffer);

	// copy the vertices into the buffer
	D3D11_MAPPED_SUBRESOURCE ms;
	devcon->Map(pVBuffer, NULL, D3D11_MAP_WRITE_DISCARD, NULL, &ms);    // map the buffer
	memcpy(ms.pData, pCoord, sizeof(Coord)* nbElts);                 // copy the data
	devcon->Unmap(pVBuffer, NULL);
}


// this function loads and prepares the shaders
void InitPipeline()
{
	// compile the shaders
	ID3D10Blob *VS, *PS;
	D3DCompileFromFile(L"shaders.hlsl", 0, 0, "VShader", "vs_5_0", 0, 0, &VS, 0);
	D3DCompileFromFile(L"shaders.hlsl", 0, 0, "PShader", "ps_5_0", 0, 0, &PS, 0);

	// create the shader objects
	dev->CreateVertexShader(VS->GetBufferPointer(), VS->GetBufferSize(), NULL, &pVS);
	dev->CreatePixelShader(PS->GetBufferPointer(), PS->GetBufferSize(), NULL, &pPS);

	// set the shader objects
	devcon->VSSetShader(pVS, 0, 0);
	devcon->PSSetShader(pPS, 0, 0);

	// create the input element object
	D3D11_INPUT_ELEMENT_DESC ied[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};

	dev->CreateInputLayout(ied, 1, VS->GetBufferPointer(), VS->GetBufferSize(), &pLayout);
	devcon->IASetInputLayout(pLayout);

	// create the constant buffer
	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));

	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = 16;
	bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;

	dev->CreateBuffer(&bd, NULL, &pCBuffer);

	devcon->VSSetConstantBuffers(0, 1, &pCBuffer);
}

void InitCBuffer(int screenWidth, int screenHeight, Limit * pLimit){
	CBUFFER cBuffer;
	float xInter = pLimit->xMaximum - pLimit->xMinimum;
	float yInter = pLimit->yMaximum - pLimit->yMinimum;

	float xCoef = 0.0f;
	float yCoef = 0.0f;

	if ((screenWidth / xInter) > (screenHeight / yInter)){
		yCoef = 2.0f / yInter;
		xCoef = yCoef * screenHeight / screenWidth;
	}
	else{
		xCoef = 2.0f / xInter;
		yCoef = xCoef * screenWidth / screenHeight;
	}

	float xAff = -1.0f - (xCoef * pLimit->xMinimum);
	float yAff = -1.0f - (yCoef * pLimit->yMinimum);

	cBuffer.xCoef = xCoef;
	cBuffer.yCoef = yCoef;
	cBuffer.xAff = xAff;
	cBuffer.yAff = yAff;

	devcon->UpdateSubresource(pCBuffer, 0, 0, &cBuffer, 0, 0);
}