#include "Ray.h"

Ray::Ray()
{
}

Ray::~Ray()
{
}

const DirectX::XMFLOAT3& Ray::getOrigin() const
{
	return m_origin;
}

DirectX::FXMVECTOR Ray::getOriginAsVector() const
{
	 return DirectX::FXMVECTOR{ m_origin.x, m_origin.y ,m_origin.z , 1 };
}

const DirectX::XMFLOAT3& Ray::getDirection() const
{
	return m_direction;
}

DirectX::FXMVECTOR Ray::getDirectionAsVector() const
{
	DirectX::FXMVECTOR f = DirectX::XMVector3Normalize({ m_direction.x, m_direction.y , m_direction.z});
	return f;
}
