#include "Light.h"

DirectionalLight::DirectionalLight()
{
	lightCamera.SetOrthographicProjectionValues(10, 10, 0.01f, 10.0f);
	lightCamera.SetPosition(0, 1, 0);
	lightCamera.SetLookAtPos({ 0, 0, 0 });
}
