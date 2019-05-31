#pragma once
#include "RenderTexture.h"
#include "..\Scenegraph And Physics\Transform.h"
#include <DirectXMath.h>
using namespace DirectX;

class Camera
{
public:
	enum class ProjectionType { PERSPECTIVE, OTHOGRAPHIC };

	Camera();
	void SetPerspectiveProjectionValues(float fovDegrees, float aspectRatio, float nearZ, float farZ);
	void SetOrthographicProjectionValues(float width, float height, float nearZ, float farZ);

	const ProjectionType GetProjectionType()const;
	const XMMATRIX GetViewDirectionMatrix()const;
	const XMMATRIX & GetProjectionMatrix() const;

	Transform transform;

private:
	ProjectionType projectionType = ProjectionType::PERSPECTIVE;
	XMMATRIX projectionMatrix;
};