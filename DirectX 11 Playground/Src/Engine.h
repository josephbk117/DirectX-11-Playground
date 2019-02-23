#pragma once
#include "WindowContainer.h"
class Engine : WindowContainer
{
public:
	bool init(HINSTANCE hInstance, const std::string& windowTitle, const std::string& windowClass, int width, int height);
	bool processMessages();
	void update();
	void renderFrame();
};

