#pragma once
#include "ErrorLogger.h"
class WindowContainer;
class RenderWindow
{
private:
	int width = 0, height = 0;
	HWND handle = NULL;
	HINSTANCE hInstance = NULL;
	std::string windowTitle = "";
	std::wstring windowTitleWide = L"";
	std::string windowClass = "";
	std::wstring windowClassWide = L"";
	void registerWindowClass();
public:
	bool init( WindowContainer* winContainer ,HINSTANCE hInstance, const std::string& windowTitle, const std::string& windowClass, int width, int height);
	bool processMessages();
	HWND GetHWND() const;
	~RenderWindow();
};

