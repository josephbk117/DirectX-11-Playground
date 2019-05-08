#include "Engine.h"

int APIENTRY wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR lpCmdLine, _In_ int cmdShow)
{
	Engine engine;
	if (!engine.init(hInstance, "DirectX Playground", "DX11 Window Class", 1200, 900))
		return -1;
	while (engine.processMessages() == true)
	{
		engine.update();
		engine.renderFrame();
	}
	return 0;
}