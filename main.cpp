#include "SpriteStruct.h"
#include <Windows.h>
#include <cmath>

SpriteStruct *spriteRect;
RECT windowRect = { 0, 0, 700, 500 };

float posX = 0.0f;
float posY = 0.0f;
float spriteSize = 32.0f;

float SPEED = 0.1f;
float WHEELSPEED = 10.f;

HDC device;
HDC bufferDevice;
HBITMAP hScreen;
HBITMAP oldBmp;
PAINTSTRUCT PStruct;
SpriteStruct points;

BITMAP bm;
HDC spriteDevice;
HBITMAP image;
bool isAutoMoving = false;
void DrawSprite(HWND hWnd, float X, float Y);
float speedX = 0.2f;
float speedY = 0.2f;

void AutoMoveSprite();

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{

	isAutoMoving = GetAsyncKeyState(VK_CONTROL);
	if (!isAutoMoving) {
		if (GetAsyncKeyState(VK_UP))
			posY -= SPEED;
		if (GetAsyncKeyState(VK_DOWN))
			posY += SPEED;
		if (GetAsyncKeyState(VK_LEFT))
			posX -= SPEED;
		if (GetAsyncKeyState(VK_RIGHT))
			posX += SPEED;

		if (uMsg == WM_MOUSEWHEEL && GET_KEYSTATE_WPARAM(wParam) == MK_SHIFT)
		{
			if (GET_WHEEL_DELTA_WPARAM(wParam) > 0)
				posX += WHEELSPEED;
			else
				posX -= WHEELSPEED;
		}
		else if (uMsg == WM_MOUSEWHEEL && GET_KEYSTATE_WPARAM(wParam) != MK_SHIFT)
		{
			if (GET_WHEEL_DELTA_WPARAM(wParam) > 0)
				posY -= WHEELSPEED;
			else
				posY += WHEELSPEED;
		}
	}


	if (uMsg == WM_DESTROY)
	{
		PostQuitMessage(0);
		return 0;
	}

	if (uMsg == WM_PAINT)
	{
		DrawSprite(hwnd, posX, posY);
	}

	return DefWindowProc(hwnd, uMsg, wParam, lParam);
}


int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE prevInstance, LPSTR cmdLine, int nCmdShow)
{
	WNDCLASSEX windowClass;
	MSG message;
	float distance = 0.0f;
		
	ZeroMemory(&windowClass, sizeof(WNDCLASSEX));
	windowClass.cbSize = sizeof(WNDCLASSEX);
	windowClass.hbrBackground = (HBRUSH)COLOR_WINDOW+1;
	windowClass.hInstance = hInstance;
	windowClass.lpfnWndProc = WindowProc;
	windowClass.lpszClassName = "MainWindow";
	windowClass.style = CS_HREDRAW | CS_VREDRAW;
	RegisterClassEx(&windowClass);

	AdjustWindowRectEx(&windowRect, WS_OVERLAPPEDWINDOW, NULL, WS_EX_OVERLAPPEDWINDOW);

	HWND winHandle = CreateWindowEx(WS_EX_OVERLAPPEDWINDOW, "MainWindow", "Sprite animation", WS_OVERLAPPEDWINDOW,
		220, 70, windowRect.right - windowRect.left, windowRect.bottom - windowRect.top, NULL, NULL, hInstance, 0);

	if (!winHandle) return -1;

	
	posX = (windowRect.right - windowRect.left) / 2.5f;
	posY = (windowRect.bottom - windowRect.top) / 2.5f;

	message.message = WM_NULL;

	device = GetDC(winHandle);
	bufferDevice = CreateCompatibleDC(device);
	windowRect = windowRect;

	hScreen = CreateCompatibleBitmap(device, windowRect.right, windowRect.bottom);
	oldBmp = (HBITMAP)SelectObject(bufferDevice, hScreen);

	image = (HBITMAP)LoadImage(0, "deer.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
	GetObject(image, sizeof(BITMAP), &bm);
	spriteDevice = CreateCompatibleDC(device);
	SelectObject(spriteDevice, image);

	SpriteStruct coordinates;
	coordinates.bottom = spriteSize;
	coordinates.left = 0;
	coordinates.right = spriteSize;
	coordinates.top = 0;

	spriteRect = &coordinates;

	ShowWindow(winHandle, nCmdShow);

	while (message.message != WM_QUIT)
	{
		GetClientRect(winHandle, &windowRect);

		if (PeekMessage(&message, NULL, 0, 0, PM_REMOVE))
		{
			DispatchMessage(&message);
		}

		if (isAutoMoving) {
			AutoMoveSprite();
		}
		else {

			if (posY - spriteRect->top <= windowRect.top) {
				posY = windowRect.top + spriteRect->top;
			}
			if (posX + spriteRect->right >= windowRect.right) {
				posX = windowRect.right - spriteRect->right;
			}
			if (posY + spriteRect->bottom >= windowRect.bottom) {
				posY = windowRect.bottom - spriteRect->bottom;
			}
			if (posX - spriteRect->left <= windowRect.left) {
				posX = windowRect.left + spriteRect->left;
			}
		}

		InvalidateRect(winHandle, NULL, FALSE);
	}

	DeleteObject(hScreen);
	DeleteObject(oldBmp);
	DeleteObject((HBITMAP)image);
	DeleteDC(bufferDevice);
	DeleteDC(device);
	DeleteDC(spriteDevice);

	return 0;
}

void DrawSprite(HWND hWnd, float X, float Y)
{
	BeginPaint(hWnd, &PStruct);
	PatBlt(bufferDevice, 0, 0, windowRect.right, windowRect.bottom, WHITENESS);

	BitBlt(bufferDevice, (int)X, (int)Y, bm.bmWidth, bm.bmHeight, spriteDevice, 0, 0, SRCAND);

	BitBlt(device, 0, 0, windowRect.right, windowRect.bottom, bufferDevice, 0, 0, SRCCOPY);

	EndPaint(hWnd, &PStruct);
}

void AutoMoveSprite() {
	if (posY - spriteRect->top <= windowRect.top) {
		speedY = -speedY;
	}
	if (posX + spriteRect->right >= windowRect.right) {
		speedX = -speedX;
	}
	if (posY + spriteRect->bottom >= windowRect.bottom) {
		speedY = -speedY;
	}
	if (posX - spriteRect->left <= windowRect.left) {
		speedX = -speedX;
	}

	posX += speedX;
	posY += speedY;
}