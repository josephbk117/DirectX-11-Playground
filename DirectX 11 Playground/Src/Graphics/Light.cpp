#include "Light.h"

DirectionalLight::DirectionalLight()
{
	lightCamera.SetOrthographicProjectionValues(12*2, 9*2, 0.1f, 100.0f);
	lightCamera.SetPosition(0, 4, 0);
	lightCamera.SetLookAtPos({ 0, 0, 0 });
}
