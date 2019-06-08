#include "Engine.h"
int APIENTRY wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR lpCmdLine, _In_ int cmdShow)
{
	Engine engine;
	if (!engine.init(hInstance, "DirectX Playground", "DX11 PLayground", 900, 900))
	{
		ErrorLogger::log("Engine initialization failed");
		return -1;
	}
	while (engine.processMessages() == true)
	{
		engine.update();
		engine.renderFrame();
	}
	return 0;
}