#include "Light.h"

DirectionalLight::DirectionalLight()
{
	lightCamera.SetOrthographicProjectionValues(14, 14, 0.01f, 10.0f);
	lightCamera.SetPosition(0, 3, 0);
	lightCamera.SetLookAtPos({ 0, 0, 0 });
}
