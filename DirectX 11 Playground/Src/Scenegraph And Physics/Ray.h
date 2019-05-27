#pragma once
#include <DirectXMath.h>

class Ray
{
private:
	DirectX::XMFLOAT3 m_origin;
	DirectX::XMFLOAT3 m_direction;
public:
	Ray();
	~Ray();
	Ray(const Ray& ref) = default;
	Ray(const DirectX::XMFLOAT3& origin, const DirectX::XMFLOAT3& direction) : m_origin(origin), m_direction(direction) {};
	const DirectX::XMFLOAT3& getOrigin()const;
	DirectX::FXMVECTOR getOriginAsVector()const;
	const DirectX::XMFLOAT3& getDirection()const;
	DirectX::FXMVECTOR getDirectionAsVector()const;
};