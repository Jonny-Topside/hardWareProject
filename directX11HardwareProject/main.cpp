// CGS HW Project A "Line Land".
// Author: L.Norri CD CGS, FullSail University
// Introduction:
// Welcome to the hardware project of the Computer Graphics Systems class.
// This assignment is fully guided but still requires significant effort on your part. 
// Future classes will demand the habits & foundation you develop right now!  
// It is CRITICAL that you follow each and every step. ESPECIALLY THE READING!!!
// TO BEGIN: Open the word document that acompanies this project and start from the top.
//************************************************************
//************ INCLUDES & DEFINES ****************************
//************************************************************
#include <iostream>
#include <ctime>
#include "XTime.h"
#include "EngineMath.h"
#include <Windows.h>
#include <WinBase.h>
//#include "includes.h"
using namespace std;
// BEGIN PART 1
// TODO: PART 1 STEP 1a
#include <d3d11.h>
// TODO: PART 1 STEP 1b
#include "D3DUtil.h"
#pragma comment (lib, "d3d11.lib")
using namespace DirectX;
#include <DirectXColors.h>
// TODO: PART 2 STEP 6
#include "Trivial_PS.csh"
#include "Trivial_VS.csh"
#define BACKBUFFER_WIDTH	500
#define BACKBUFFER_HEIGHT	500
//************************************************************
//************ SIMPLE WINDOWS APP CLASS **********************
//************************************************************
class DEMO_APP
{
	struct SIMPLE_VERTEX
	{
		XMFLOAT2 pos;
	};
	struct SIMPLE_TRIANGLE
	{
		SIMPLE_VERTEX a, b, c;
	};
	struct SEND_TO_VRAM
	{
		XMFLOAT4 constColor;
		XMFLOAT2 constOffset;
		XMFLOAT2 constPadding;
	};

	HINSTANCE						application;
	WNDPROC							appWndProc;
	HWND							window;
	// TODO: PART 1 STEP 2
	ID3D11Device * m_pDevice = nullptr;
	ID3D11DeviceContext * m_pImmediateContext = nullptr;
	IDXGISwapChain * m_pSwapChain = nullptr;
	ID3D11RenderTargetView * m_pRenderTargetView = nullptr;
	ID3D11Resource * resource = nullptr;
	SIMPLE_VERTEX velocityVector;

	ID3D11Buffer* buff = nullptr;
	//ID3D11Buffer* vertexBuffer;
	ID3D11Buffer* constantBuffer = nullptr;

	D3D11_BUFFER_DESC buffDesc;
	D3D11_BUFFER_DESC constantBuffDesc;

	ID3D11InputLayout* inputLayout;
	float degrees;
	D3D_DRIVER_TYPE m_DriverType;
	D3D_FEATURE_LEVEL m_FeatureLevel;
	D3D11_VIEWPORT m_Viewport;
	//square gridSquares[200];
	// TODO: PART 2 STEP 2
	unsigned int numOfVerts = 0;
	SIMPLE_VERTEX tempVert;
	SIMPLE_VERTEX simp[361];
	// BEGIN PART 5
	// TODO: PART 5 STEP 1
	D3D11_BUFFER_DESC squareGridBufferDesc;
	ID3D11Buffer* squareGridBuffer = nullptr;
	int triangleCount = 400;
	// TODO: PART 2 STEP 4
	ID3D11VertexShader *vertexShader;
	ID3D11PixelShader *pixelShader;
	// BEGIN PART 3
	// TODO: PART 3 STEP 1
	XTime timer;

	// TODO: PART 3 STEP 2b

	// TODO: PART 3 STEP 4a

public:
	// BEGIN PART 2
	// TODO: PART 2 STEP 1

	SEND_TO_VRAM toShader;
	
	SEND_TO_VRAM squareGridToShader;


	DEMO_APP(HINSTANCE hinst, WNDPROC proc);
	bool Run();
	bool ShutDown();
};

//************************************************************
//************ CREATION OF OBJECTS & RESOURCES ***************
//************************************************************

DEMO_APP::DEMO_APP(HINSTANCE hinst, WNDPROC proc)
{
	// ****************** BEGIN WARNING ***********************// 
	// WINDOWS CODE, I DON'T TEACH THIS YOU MUST KNOW IT ALREADY! 
	
		application = hinst;
		appWndProc = proc;

		WNDCLASSEX  wndClass;
		ZeroMemory(&wndClass, sizeof(wndClass));
		wndClass.cbSize = sizeof(WNDCLASSEX);
		wndClass.lpfnWndProc = appWndProc;
		wndClass.lpszClassName = L"DirectXApplication";
		wndClass.hInstance = application;
		wndClass.hCursor = LoadCursor(NULL, IDC_ARROW);
		wndClass.hbrBackground = (HBRUSH)(COLOR_WINDOWFRAME);
		//wndClass.hIcon			= LoadIcon(GetModuleHandle(NULL), MAKEINTRESOURCE(IDI_FSICON));
		RegisterClassEx(&wndClass);

		RECT window_size = { 0, 0, BACKBUFFER_WIDTH, BACKBUFFER_HEIGHT };
		AdjustWindowRect(&window_size, WS_OVERLAPPEDWINDOW, false);

		window = CreateWindow(L"DirectXApplication", L"CGS Hardware Project", WS_OVERLAPPEDWINDOW & ~(WS_THICKFRAME | WS_MAXIMIZEBOX),
			CW_USEDEFAULT, CW_USEDEFAULT, window_size.right - window_size.left, window_size.bottom - window_size.top,
			NULL, NULL, application, this);

		ShowWindow(window, SW_SHOW);
		//********************* END WARNING ************************//
	
	// TODO: PART 1 STEP 3a
	m_pDevice = nullptr;
	m_pImmediateContext = nullptr;
	m_pRenderTargetView = nullptr;
	m_pSwapChain = nullptr;
	
	UINT createDeviceFlags = 0;
	// TODO: PART 1 STEP 3b
	if (_DEBUG)
	{
	 createDeviceFlags = D3D11_CREATE_DEVICE_DEBUG;
	}
	D3D_DRIVER_TYPE driverTypes[] =
	{
		D3D_DRIVER_TYPE_HARDWARE,
		D3D_DRIVER_TYPE_WARP,
		D3D_DRIVER_TYPE_REFERENCE
	};

	UINT numDriverTypes = ARRAYSIZE(driverTypes);

	D3D_FEATURE_LEVEL featureLevels[] =
	{
	D3D_FEATURE_LEVEL_11_0,
	D3D_FEATURE_LEVEL_10_1,
	D3D_FEATURE_LEVEL_10_0,
	D3D_FEATURE_LEVEL_9_3
	};


	UINT numFeatureLevels = ARRAYSIZE(featureLevels);


	DXGI_SWAP_CHAIN_DESC swapDesc;
	//DXGI_SAMPLE_DESC *sdsc;
	ZeroMemory(&swapDesc, sizeof(DXGI_SWAP_CHAIN_DESC));
	//SWAP DESCRIPTION
	velocityVector.pos.x = 1.0f;
	velocityVector.pos.y = 0.5f;


		swapDesc.BufferCount = 1;
		swapDesc.BufferDesc.Width = BACKBUFFER_WIDTH;
		swapDesc.BufferDesc.Height = BACKBUFFER_HEIGHT;
		swapDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		swapDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		swapDesc.OutputWindow = window;
		swapDesc.Windowed = true;
		swapDesc.SampleDesc.Count = 1;
		swapDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

		unsigned int flag = 0;

		if (_DEBUG)
			flag = D3D11_CREATE_DEVICE_DEBUG;


	D3D11CreateDeviceAndSwapChain(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, D3D11_CREATE_DEVICE_DEBUG,
			NULL, NULL, D3D11_SDK_VERSION, &swapDesc, &m_pSwapChain, &m_pDevice,
			NULL, &m_pImmediateContext);
		
	

	ID3D11Texture2D* m_pBackBufferTex = 0;
	{
	m_pImmediateContext->OMSetRenderTargets(1, &m_pRenderTargetView, nullptr);
	}

	// TODO: PART 1 STEP 4
	m_pSwapChain->GetBuffer(NULL, __uuidof(resource), reinterpret_cast<void**>(&resource));
	m_pDevice->CreateRenderTargetView(resource, nullptr, &m_pRenderTargetView);

	// TODO: PART 1 STEP 5
	//SETTING UP VIEWPORT
	{

		m_Viewport.Width = BACKBUFFER_WIDTH;
		m_Viewport.Height = BACKBUFFER_HEIGHT;
		m_Viewport.TopLeftX = 0;
		m_Viewport.TopLeftY = 0;
		m_Viewport.MinDepth = 0;
		m_Viewport.MaxDepth = 1;
	//	m_pImmediateContext->RSSetViewports(1, &m_Viewport);
	
	}
	
	toShader.constOffset.x = 0;
	toShader.constOffset.y = 0;
	toShader.constColor.w = 255;
	toShader.constColor.x = 255;
	toShader.constColor.y = 255;
	toShader.constColor.z = 255;

	
	// TODO: PART 2 STEP 3a
	for (unsigned int i = 0; i < 361; i++)
	{
		float rad = XMConvertToRadians((float)i);
		simp[i].pos.x = cos(rad);
		simp[i].pos.y = sin(rad);
		numOfVerts++;
	}

	// BEGIN PART 4
	// TODO: PART 4 STEP 1
	for (unsigned int i = 0; i < 361; i++)
	{
		simp[i].pos.x *= 0.2;
		simp[i].pos.y *= 0.2;
	}
	// TODO: PART 2 STEP 3b
	buffDesc.Usage = D3D11_USAGE_DYNAMIC;
	buffDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	buffDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	buffDesc.ByteWidth = sizeof(SIMPLE_VERTEX) * 361;
	buffDesc.MiscFlags = 0;

	
	constantBuffDesc.Usage = D3D11_USAGE_DYNAMIC;
	constantBuffDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	constantBuffDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	constantBuffDesc.ByteWidth = sizeof(SEND_TO_VRAM);
	constantBuffDesc.StructureByteStride = 0;
	constantBuffDesc.MiscFlags = 0;

	squareGridBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	squareGridBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	squareGridBufferDesc.CPUAccessFlags = NULL;
	squareGridBufferDesc.ByteWidth = sizeof(SIMPLE_TRIANGLE) * 400;
	squareGridBufferDesc.StructureByteStride = 0;
	squareGridBufferDesc.MiscFlags = NULL;
	// TODO: PART 2 STEP 3c
	D3D11_SUBRESOURCE_DATA srd = { simp, 0, 0 };
	D3D11_SUBRESOURCE_DATA constantSrd = { &toShader, 0, 0 };
	// TODO: PART 2 STEP 3d
	   m_pDevice->CreateBuffer(&buffDesc, &srd, &buff);
	 m_pDevice->CreateBuffer(&constantBuffDesc, NULL, &constantBuffer);
	// TODO: PART 5 STEP 2a
	 SIMPLE_TRIANGLE gridTriangles[400];
	 SIMPLE_VERTEX topLeft, topRight, bottomLeft, bottomRight;
	 topLeft.pos.x = -1.0f;
	 topLeft.pos.y =  1.0f;
	 topRight.pos.x = -0.9f;
	 topRight.pos.y = 1.0f;
	 bottomLeft.pos.x = -1.0f;
	 bottomLeft.pos.y = 0.9f;
	 bottomRight.pos.x = -0.9f;
	 bottomRight.pos.y = 0.9f;

	//FOR DRAWING THE SQUARES ON THE GRID
	//10 ACROSS, 20 DOWN
	//THERE IS NO SQUARE AT THE TOP LEFT OR BOTTOM RIGHT CORNER
	//WOULD IT BE POSSIBLE TO MAKE AN ARRAY OF 1200 VERTICES, THEN CREATE THE FIRST SQUARE, THEN INCREMENT THE POSITION OF THOSE VERTS TO + 0.1 FOR INSTANCE?
	for (unsigned int i = -1.0f; i <= 1.0f; i += 0.2)
	{

	}

	// TODO: PART 5 STEP 2b
	bool whenToDraw = false;
	int indexCounter = 0;
	for (unsigned int y = 0; y < 20; y++)
	{
		for (unsigned int x = 0; x < 20; x++)
		{
			if (whenToDraw == true)
			{
				gridTriangles[indexCounter].a.pos.x = topLeft.pos.x + 0.1f * x;
				gridTriangles[indexCounter].a.pos.y = topLeft.pos.y - 0.1f * y;

				gridTriangles[indexCounter].b.pos.x = bottomRight.pos.x + 0.1f * x;
				gridTriangles[indexCounter].b.pos.y = bottomRight.pos.y - 0.1f * y;

				gridTriangles[indexCounter].c.pos.x = bottomLeft.pos.x + 0.1f * x;
				gridTriangles[indexCounter].c.pos.y = bottomLeft.pos.y - 0.1f * y;

				gridTriangles[indexCounter + 1].a.pos.x = topLeft.pos.x + 0.1f * x;
				gridTriangles[indexCounter + 1].a.pos.y = topLeft.pos.y - 0.1f * y;

				gridTriangles[indexCounter + 1].b.pos.x = topRight.pos.x + 0.1f * x;
				gridTriangles[indexCounter + 1].b.pos.y = topRight.pos.y - 0.1f * y;

				gridTriangles[indexCounter + 1].c.pos.x = bottomRight.pos.x + 0.1f * x;
				gridTriangles[indexCounter + 1].c.pos.y = bottomRight.pos.y - 0.1f * y;

				indexCounter += 2;
			}
			whenToDraw = !whenToDraw;
		}
		whenToDraw = !whenToDraw;
	}
	// TODO: PART 5 STEP 3
	D3D11_SUBRESOURCE_DATA squareGridSRD = { &gridTriangles, 0, 0 };

	m_pDevice->CreateBuffer(&squareGridBufferDesc, &squareGridSRD, &squareGridBuffer);

	// TODO: PART 2 STEP 5
	// ADD SHADERS TO PROJECT, SET BUILD OPTIONS & COMPILE

	// TODO: PART 2 STEP 7
	m_pDevice->CreateVertexShader(Trivial_VS, sizeof(Trivial_VS), nullptr, &vertexShader);
	 m_pDevice->CreatePixelShader(Trivial_PS, sizeof(Trivial_PS), nullptr, &pixelShader);
	// TODO: PART 2 STEP 8a
	D3D11_INPUT_ELEMENT_DESC vLayout[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		
	};



	// TODO: PART 2 STEP 8b
	m_pDevice->CreateInputLayout(vLayout, 1, Trivial_VS, sizeof(Trivial_VS), &inputLayout);
	// TODO: PART 3 STEP 3

	// TODO: PART 3 STEP 4b

}

//************************************************************
//************ EXECUTION *************************************
//************************************************************

bool DEMO_APP::Run()
{
	UINT stride = sizeof(SIMPLE_VERTEX);
	UINT offsets = 0;

	// TODO: PART 4 STEP 2	
	timer.Signal();
	//[0] is RIGHTMOST POINT
	// TODO: PART 4 STEP 3
	static XMFLOAT2 velocity(1, 0.5);

	toShader.constOffset.x += velocity.x * timer.Delta();
	toShader.constOffset.y += velocity.y * timer.Delta();

	// TODO: PART 4 STEP 5
	if (toShader.constOffset.x + 0.2 > 1 || toShader.constOffset.x - 0.2 < -1)
	{
		velocity.x *= -1;
	}
	if (toShader.constOffset.y + 0.2 > 1 || toShader.constOffset.y - 0.2 < -1)
	{
		velocity.y *= -1;
	}
	
	// TODO: PART 4 STEP 5
	

	// END PART 4

	// TODO: PART 1 STEP 7a
	m_pImmediateContext->OMSetRenderTargets(1, &m_pRenderTargetView, nullptr);

	// TODO: PART 1 STEP 7b
	m_pImmediateContext->RSSetViewports(1, &m_Viewport);

	// TODO: PART 1 STEP 7c
	const float color[4] = { 0.0f, 0.0f, 1.0f, 1.0f };
	m_pImmediateContext->ClearRenderTargetView(m_pRenderTargetView, color);

	// TODO: PART 5 STEP 4
	squareGridToShader.constColor.w = 0xFF;
	squareGridToShader.constColor.x = 0x00;
	squareGridToShader.constColor.y = 0x00;
	squareGridToShader.constColor.z = 0x00;
	squareGridToShader.constOffset.x = 0.0f;
	squareGridToShader.constOffset.y = 0.0f;

	// TODO: PART 5 STEP 5
	D3D11_MAPPED_SUBRESOURCE mappedSubresource;


	m_pImmediateContext->Map(constantBuffer, NULL, D3D11_MAP_WRITE_DISCARD, NULL, &mappedSubresource);
	memcpy(mappedSubresource.pData, &squareGridToShader, sizeof(squareGridToShader));

	m_pImmediateContext->Unmap(constantBuffer, NULL);

	m_pImmediateContext->IASetVertexBuffers(0, 1, &squareGridBuffer, &stride, &offsets);
	// TODO: PART 5 STEP 6
	m_pImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);



	// TODO: PART 2 STEP 9b
	m_pImmediateContext->VSSetShader(vertexShader, nullptr, 0);
	m_pImmediateContext->PSSetShader(pixelShader, nullptr, 0);
	// TODO: PART 2 STEP 9c
	m_pImmediateContext->IASetInputLayout(inputLayout);

	// TODO: PART 5 STEP 7
	m_pImmediateContext->Draw(1200, 0);
	// END PART 5

	// TODO: PART 3 STEP 5
	D3D11_MAPPED_SUBRESOURCE subMap;

	m_pImmediateContext->Map(constantBuffer, NULL, D3D11_MAP_WRITE_DISCARD, NULL, &subMap);
	memcpy(subMap.pData, &toShader, sizeof(toShader));
	m_pImmediateContext->Unmap(constantBuffer, NULL);
	//m_pImmediateContext->UpdateSubresource(constantBuffer, 0, NULL, &toShader, 0, 0);
	//m_pImmediateContext->Map(constantBuffer, 0, mapper, 0, &subMap);
	//m_pImmediateContext->Unmap(constantBuffer, 0);
	// TODO: PART 3 STEP 6
	m_pImmediateContext->VSSetConstantBuffers(0, 1, &constantBuffer);
	

	stride = sizeof(SIMPLE_VERTEX);
	offsets = 0;
	//m_pImmediateContext->VSSetConstantBuffers(0, 1, &squareGridBuffer);

	// TODO: PART 2 STEP 9a
	m_pImmediateContext->IASetVertexBuffers(0, 1, &buff, &stride, &offsets);
	// TODO: PART 2 STEP 9d
	m_pImmediateContext->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_LINESTRIP);
	// TODO: PART 2 STEP 10
	
	m_pImmediateContext->Draw(361,0);

	// END PART 2

	// TODO: PART 1 STEP 8
	m_pSwapChain->Present(1, 0);

	// END OF PART 1
	return true;
}

//************************************************************
//************ DESTRUCTION ***********************************
//************************************************************

bool DEMO_APP::ShutDown()
{

	// TODO: PART 1 STEP 6
	

	UnregisterClass(L"DirectXApplication", application);
	m_pRenderTargetView->Release();
	m_pImmediateContext->Release();
	m_pSwapChain->Release();
	m_pDevice->Release();
	
	return true;
}

//************************************************************
//************ WINDOWS RELATED *******************************
//************************************************************

// ****************** BEGIN WARNING ***********************// 
// WINDOWS CODE, I DON'T TEACH THIS YOU MUST KNOW IT ALREADY!

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpCmdLine, int nCmdShow);
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wparam, LPARAM lparam);

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE, LPTSTR, int)
{
	srand(unsigned int(time(0)));
	DEMO_APP myApp(hInstance, (WNDPROC)WndProc);
	MSG msg; ZeroMemory(&msg, sizeof(msg));
	while (msg.message != WM_QUIT && myApp.Run())
	{
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}
	myApp.ShutDown();
	return 0;
}
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	if (GetAsyncKeyState(VK_ESCAPE))
		message = WM_DESTROY;
	switch (message)
	{
	case (WM_DESTROY): { PostQuitMessage(0); }
					   break;
	}
	return DefWindowProc(hWnd, message, wParam, lParam);
}
//********************* END WARNING ************************//

