#include "WindowContainer.h"


bool RenderWindow::init(WindowContainer* winContainer, HINSTANCE hInstance, const std::string & windowTitle, const std::string & windowClass, int width, int height)
{
	this->hInstance = hInstance;
	this->windowTitle = windowTitle;
	this->windowTitleWide = StringConverter::StringToWide(windowTitle);
	this->windowClass = windowClass;
	this->windowClassWide = StringConverter::StringToWide(windowClass);
	this->width = width;
	this->height = height;

	this->registerWindowClass();

	//Window rectangle
	RECT wr;
	wr.left = 50;
	wr.top = 50;
	wr.right = wr.left + width;
	wr.bottom = wr.top + height;

	AdjustWindowRect(&wr, WS_CAPTION | WS_MAXIMIZEBOX | WS_SYSMENU, false);

	this->handle = CreateWindowEx(0, this->windowClassWide.c_str(), this->windowTitleWide.c_str(),
		WS_CAPTION | WS_MAXIMIZEBOX | WS_SYSMENU, wr.left, wr.top,
		wr.right - wr.left, wr.bottom - wr.top, NULL, NULL, this->hInstance, winContainer);

	if (this->handle == NULL)
	{
		ErrorLogger::log(GetLastError(), "Window Creation Error");
		return false;
	}

	ShowWindow(this->handle, SW_SHOW);
	SetForegroundWindow(this->handle);
	SetFocus(this->handle);
	return true;
}

bool RenderWindow::processMessages()
{
	MSG msg;

	ZeroMemory(&msg, sizeof(MSG));

	if (PeekMessage(&msg, handle, 0, 0, PM_REMOVE))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	if (msg.message == WM_NULL)
	{
		if (!IsWindow(handle))
		{
			handle = NULL;
			UnregisterClass(windowClassWide.c_str(), hInstance);
			return false;
		}
	}

	return true;
}

HWND RenderWindow::GetHWND() const
{
	return handle;
}


LRESULT CALLBACK HandleMsgRedirect(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
		// All other messages
	case WM_CLOSE:
		DestroyWindow(hwnd);
		return 0;

	default:
	{
		// retrieve ptr to window class
		WindowContainer* const pWindow = reinterpret_cast<WindowContainer*>(GetWindowLongPtr(hwnd, GWLP_USERDATA));
		// forward message to window class handler
		return pWindow->WindowProc(hwnd, uMsg, wParam, lParam);
	}
	}
}

LRESULT CALLBACK HandleMessageSetup(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
	case WM_NCCREATE:
	{
		const CREATESTRUCTW* const pCreate = reinterpret_cast<CREATESTRUCTW*>(lParam);
		WindowContainer * pWindow = reinterpret_cast<WindowContainer*>(pCreate->lpCreateParams);
		if (pWindow == nullptr) //Sanity check
		{
			ErrorLogger::log("Critical Error: Pointer to window container is null during WM_NCCREATE.");
			exit(-1);
		}
		SetWindowLongPtr(hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(pWindow));
		SetWindowLongPtr(hwnd, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(HandleMsgRedirect));
		return pWindow->WindowProc(hwnd, uMsg, wParam, lParam);
	}
	default:
		return DefWindowProc(hwnd, uMsg, wParam, lParam);
	}
}

void RenderWindow::registerWindowClass()
{
	WNDCLASSEX wc;
	wc.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
	wc.lpfnWndProc = HandleMessageSetup;//DefWindowProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = this->hInstance;
	wc.hIcon = NULL;
	wc.hIconSm = NULL;
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = NULL;
	wc.lpszMenuName = NULL;
	wc.lpszClassName = this->windowClassWide.c_str();
	wc.cbSize = sizeof(WNDCLASSEX);
	RegisterClassEx(&wc);
}

RenderWindow::~RenderWindow()
{
	if (handle != NULL)
	{
		UnregisterClass(windowClassWide.c_str(), hInstance);
		DestroyWindow(handle);
	}
}

