#include "Particle.h"

Particle::Particle(const DirectX::XMFLOAT3& pos, const DirectX::XMFLOAT3& velocity, float gravity, float lifeTime, float rotation, float scale)
{
	m_pos = pos;
	m_velocity = velocity;
	m_gravity = gravity;
	m_lifeTime = lifeTime;
	m_rotation = rotation;
	m_scale = scale;
}

DirectX::XMFLOAT3 Particle::getPosition() const
{
	return m_pos;
}

float Particle::getRotation() const
{
	return m_rotation;
}

float Particle::getScale() const
{
	return m_scale;
}

bool Particle::getActiveState() const
{
	return m_isActive;
}

void Particle::setActiveState(bool isActive)
{
	m_isActive = isActive;
}

void Particle::resetStates(const DirectX::XMFLOAT3& pos, const DirectX::XMFLOAT3& velocity, float gravity, float lifeTime, float rotation, float scale)
{
	m_pos = pos;
	m_velocity = velocity;
	m_gravity = gravity;
	m_lifeTime = lifeTime;
	m_rotation = rotation;
	m_scale = scale;
	m_elapsedTime = 0;
}

bool Particle::update(float deltaTime)
{
	DirectX::XMVECTOR velocity = DirectX::XMLoadFloat3(&m_velocity);
	DirectX::XMVECTOR position = DirectX::XMLoadFloat3(&m_pos);

	velocity.m128_f32[1] -= m_gravity * deltaTime;
	position = DirectX::XMVectorAdd(position, DirectX::XMVectorScale(velocity, deltaTime));

	DirectX::XMStoreFloat3(&m_velocity, velocity);
	DirectX::XMStoreFloat3(&m_pos, position);

	m_rotation += deltaTime * 2.0f;

	m_elapsedTime += deltaTime;
	return m_elapsedTime <= m_lifeTime;
}
