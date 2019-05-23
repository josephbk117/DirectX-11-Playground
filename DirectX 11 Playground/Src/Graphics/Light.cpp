#include "Light.h"

DirectionalLight::DirectionalLight()
{
	lightCamera.SetOrthographicProjectionValues(12*1.5, 9*1.5, 0.01f, 10.0f);
	lightCamera.SetPosition(0, 1, 0);
	lightCamera.SetLookAtPos({ 0, 0, 0 });
}
