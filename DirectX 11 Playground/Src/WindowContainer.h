#pragma once
#include "RenderWindow.h"
#include "Keyboard\KeyboardInput.h"
#include "Mouse\MouseInput.h"
#include "Graphics\Graphics.h"

class WindowContainer
{
public:
	LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
protected:
	RenderWindow renderWindow;
	KeyboardInput keyboard;
	MouseInput mouse;
	Graphics gfx;
};

