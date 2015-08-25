#include <memory>
#include <Windows.h>
#include "MyDemo.h"

#define WINDOW_NAME		"My Demo"

const int WindowWidth = 800;
const int WindowHeight = 600;
const int GameFrameRate = 60;

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

//Global variables
HINSTANCE HInstance = 0;
HWND Hwnd = 0;

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE prevInstance, LPWSTR cmdLine, int cmdShow)
{
	HInstance = hInstance;
	//Prevent warning from compiler
	UNREFERENCED_PARAMETER(prevInstance);
	UNREFERENCED_PARAMETER(hInstance);
	UNREFERENCED_PARAMETER(cmdLine);

	//Register window class involved in style setting
	WNDCLASSEX wndClass = { 0 };
	wndClass.cbSize = sizeof(WNDCLASSEX);
	wndClass.style = CS_HREDRAW | CS_VREDRAW;
	wndClass.lpfnWndProc = WndProc;
	wndClass.hInstance = hInstance;
	wndClass.hCursor = LoadCursor(NULL, IDC_ARROW);
	wndClass.hbrBackground = (HBRUSH)(GetStockObject(BLACK_BRUSH));
	wndClass.lpszMenuName = NULL;
	wndClass.lpszClassName = WINDOW_NAME;

	if (!RegisterClassEx(&wndClass))
		return -1;

	//Create window
	RECT rc = { 0, 0, WindowWidth, WindowHeight };
	AdjustWindowRect(&rc, WS_OVERLAPPEDWINDOW, FALSE);
	Hwnd = CreateWindowA(WINDOW_NAME, WINDOW_NAME,
		WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, 
		rc.right - rc.left, rc.bottom - rc.top, NULL, NULL, hInstance, NULL);
	if (!Hwnd)
		return -1;

	//Show window
	ShowWindow(Hwnd, cmdShow);

	//Initialize MyDemo
	std::unique_ptr<Game> myDemo(new MyDemo());
	bool result = myDemo->Init();
	if (!result)
		return -1;

	//Message loop
	MSG msg = { 0 };
	while (msg.message != WM_QUIT)
	{
		if (PeekMessage(&msg, 0, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else
		{
			//Determine the time that last frame cost
			static DWORD d1, d2;
			static float dt;
			d1 = GetTickCount();
			//Update and render game
			myDemo->Update(dt);
			myDemo->Render(dt);
			d2 = GetTickCount();
			//Fix time step
			while (d2 - d1 < 500.0 / GameFrameRate)
			{
				Sleep(1);
				d2 = GetTickCount();
			}
			dt = (d2 - d1) / 1000.0f;
		}
	}
	myDemo->Shutdown();
	return static_cast<int>(msg.wParam);
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message) 
	{
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hwnd, message, wParam, lParam);
	}
	return 0;
}