#pragma once
#include "Particle.h"
#include "../Material.h"
#include "../Mesh.h"
#include <vector>

struct ParticleSystemSettings
{
public:
	unsigned int maxParticles; // Total number of particles involved
	float maxLifetimeForParticle; // Time in seconds that a particle will be active in the simulation
	float emissionRate; // Particles emitted per second of simulation
	float gravity; // Gravitational force acting on particles, eg :-9.81f (Earth)
	Material* material; // The material used to display the particles
};

class ParticleSystem
{
private:
	std::vector<Particle> m_particles;
	ParticleSystemSettings m_settings;
	Mesh* m_mesh = nullptr;
	VertexConstantBuffer<CB_VS_VertexShader> m_constantBuffer;
public:
	void init(ID3D11Device* device, ID3D11DeviceContext* context, const ParticleSystemSettings& settings);
	void start();
	void update(float deltaTime);
	void draw(ID3D11DeviceContext* context, const XMMATRIX& viewProjectionMatrix, VertexConstantBuffer<CB_VS_VertexShader>& constantBuffer);
	void stop();
	~ParticleSystem();
};