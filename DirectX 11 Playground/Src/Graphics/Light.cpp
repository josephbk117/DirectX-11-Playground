#include "Light.h"

DirectionalLight::DirectionalLight()
{
	lightCamera.SetOrthographicProjectionValues(14, 14, 0.01f, 10.0f);
	lightCamera.transform.SetPosition(0, 1, 0);
	lightCamera.transform.SetLookAtPos({ 0, 0, 0 });
}
