#include "Engine.h"

int APIENTRY wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR lpCmdLine, _In_ int cmdShow)
{
	Engine engine;
	if (!engine.init(hInstance, "Magic man", "DX11 Window Class", 800, 600))
		return -1;
	while (engine.processMessages() == true)
	{
		engine.update();
		engine.renderFrame();
		//Sleep(1);
	}
	return 0;
}