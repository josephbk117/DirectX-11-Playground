#include "WindowContainer.h"

LRESULT WindowContainer::WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
	case WM_KEYDOWN:
	{
		unsigned char ch = static_cast<unsigned char>(wParam);
		if (keyboard.IsKeysAutoRepeat())
			keyboard.OnKeyPressed(ch);
		else
		{
			const bool wasPressed = lParam & 0x40000000;
			if (!wasPressed)
				keyboard.OnKeyPressed(ch);
		}
		return 0;
	}
	case WM_KEYUP:
	{
		unsigned char ch = static_cast<unsigned char>(wParam);
		keyboard.OnKeyReleased(ch);
		return 0;
	}
	case WM_CHAR:
	{
		unsigned char ch = static_cast<unsigned char>(wParam);
		if (keyboard.IsKeysAutoRepeat())
			keyboard.OnChar(ch);
		else
		{
			const bool wasPressed = lParam & 0x40000000;
			if (!wasPressed)
				keyboard.OnChar(ch);
		}
		return 0;
	}
	case WM_MOUSEMOVE:
	{
		int x = LOWORD(lParam);
		int y = HIWORD(lParam);

		mouse.OnMouseMove(x, y);
		return 0;
	}

	case WM_LBUTTONDOWN:
	{
		int x = LOWORD(lParam);
		int y = HIWORD(lParam);

		mouse.OnLeftPressed(x, y);
		return 0;
	}
	case WM_RBUTTONDOWN:
	{
		int x = LOWORD(lParam);
		int y = HIWORD(lParam);

		mouse.OnRightPressed(x, y);
		return 0;
	}
	case WM_MBUTTONDOWN:
	{
		int x = LOWORD(lParam);
		int y = HIWORD(lParam);

		mouse.OnMiddlePressed(x, y);
		return 0;
	}
	case WM_LBUTTONUP:
	{
		int x = LOWORD(lParam);
		int y = HIWORD(lParam);

		mouse.OnLeftReleased(x, y);
		return 0;
	}
	case WM_RBUTTONUP:
	{
		int x = LOWORD(lParam);
		int y = HIWORD(lParam);

		mouse.OnRightReleased(x, y);
		return 0;
	}
	case WM_MBUTTONUP:
	{
		int x = LOWORD(lParam);
		int y = HIWORD(lParam);

		mouse.OnMiddleReleased(x, y);
		return 0;
	}
	default:
		return DefWindowProc(hwnd, uMsg, wParam, lParam);
	}
}
