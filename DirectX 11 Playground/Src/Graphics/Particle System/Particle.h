#pragma once
#include <DirectXMath.h>
class Particle
{
private:
	DirectX::XMFLOAT3 m_pos = { 0,0,0 };
	DirectX::XMFLOAT3 m_velocity = { 0,0,0 };
	float m_gravity = -9.81f;
	float m_lifeTime = 0.0f;
	float m_rotation = 0.0f;
	float m_scale = 1.0f;
	float m_elapsedTime = 0;
	bool m_isActive = false;
public:
	Particle() {};
	Particle(const DirectX::XMFLOAT3& pos, const DirectX::XMFLOAT3& velocity, float gravity, float lifeTime, float rotation, float scale);
	DirectX::XMFLOAT3 getPosition()const;
	float getRotation()const;
	float getScale()const;
	bool getActiveState()const;
	void setActiveState(bool isActive);
	void resetStates(const DirectX::XMFLOAT3& pos, const DirectX::XMFLOAT3& velocity, float gravity, float lifeTime, float rotation, float scale);
	bool update(float deltaTime);
};