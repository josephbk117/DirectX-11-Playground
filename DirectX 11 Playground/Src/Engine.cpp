#include "Engine.h"

bool Engine::init(HINSTANCE hInstance, const std::string & windowTitle, const std::string & windowClass, int width, int height)
{
	timer.start();
	CoInitialize(NULL);
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
	float deltaTime = timer.getMillisecondsElapsed();
	timer.restart();

	while (!keyboard.CharBufferIsEmpty())
	{
		unsigned char ch = keyboard.ReadChar();
	}

	while (!keyboard.KeyBufferIsEmpty())
	{
		KeyboardEvent kbe = keyboard.ReadKey();
		unsigned char keycode = kbe.GetKeyCode();
	}

	while (!mouse.EventBufferIsEmpty())
	{
		MouseEvent me = mouse.ReadEvent();
		if (mouse.IsRightDown())
		{
			if (me.GetType() == MouseEvent::EventType::Move)
			{
				static int prevMouseXpos = me.GetPosX();
				static int prevMouseYpos = me.GetPosY();

				float deltaX = static_cast<float>(me.GetPosX() - prevMouseXpos) * 0.0025f;
				float deltaY = static_cast<float>(me.GetPosY() - prevMouseYpos) * 0.0025f;

				prevMouseXpos = me.GetPosX();
				prevMouseYpos = me.GetPosY();
				this->gfx.camera.AdjustRotation(0, deltaX, 0);
				this->gfx.camera.AdjustRotation(deltaY, 0, 0);
			}
		}
	}
	const float cameraSpeed = 0.002f * deltaTime;

	if (keyboard.KeyIsPressed('W'))
	{
		this->gfx.camera.AdjustPosition(this->gfx.camera.GetForwardVector() * cameraSpeed);
	}
	if (keyboard.KeyIsPressed('S'))
	{
		this->gfx.camera.AdjustPosition(this->gfx.camera.GetBackwardVector() * cameraSpeed);
	}
	if (keyboard.KeyIsPressed('A'))
	{
		this->gfx.camera.AdjustPosition(this->gfx.camera.GetLeftVector() * cameraSpeed);
	}
	if (keyboard.KeyIsPressed('D'))
	{
		this->gfx.camera.AdjustPosition(this->gfx.camera.GetRightVector() * cameraSpeed);
	}
	if (keyboard.KeyIsPressed(VK_SPACE))
	{
		this->gfx.camera.AdjustPosition(0.0f, cameraSpeed, 0.0f);
	}
	if (keyboard.KeyIsPressed('Z'))
	{
		this->gfx.camera.AdjustPosition(0.0f, -cameraSpeed, 0.0f);
	}
	

}

void Engine::renderFrame()
{
	gfx.renderFrame();
}
