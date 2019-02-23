#include "Engine.h"

bool Engine::init(HINSTANCE hInstance, const std::string & windowTitle, const std::string & windowClass, int width, int height)
{
	if (!renderWindow.init(this, hInstance, windowTitle, windowClass, width, height))
		return false;
	if (!gfx.init(renderWindow.GetHWND(), width, height))
		return false;
	return true;
}

bool Engine::processMessages()
{
	return renderWindow.processMessages();
}

void Engine::update()
{
	while (!keyboard.CharBufferIsEmpty())
	{
		unsigned char ch = keyboard.ReadChar();

		std::string debugStr = "Char : ";
		debugStr += ch;
		debugStr += '\n';
		OutputDebugStringA(debugStr.c_str());
	}
	while (!keyboard.KeyBufferIsEmpty())
	{
		unsigned char ch = keyboard.ReadKey().GetKeyCode();
		std::string debugStr = "Keycode : ";
		debugStr += ch;
		debugStr += '\n';
		OutputDebugStringA(debugStr.c_str());
	}
	while (!mouse.EventBufferIsEmpty())
	{
		MouseEvent e = mouse.ReadEvent();
		std::string debugStr = "\nX :";
		debugStr += std::to_string(e.GetPosX());
		debugStr += ", ";
		debugStr += std::to_string(e.GetPosY());
		OutputDebugStringA(debugStr.c_str());
	}
}

void Engine::renderFrame()
{
	gfx.renderFrame();
}
