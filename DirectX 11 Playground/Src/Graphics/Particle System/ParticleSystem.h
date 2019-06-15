#pragma once
#include "Particle.h"
#include "../Material.h"
#include "../Mesh.h"
#include "../Camera.h"
#include <vector>
#include <list>
struct ParticleSystemSettings
{
public:
	unsigned int maxParticles; // Total number of particles involved
	float maxLifetimeForParticle; // Time in seconds that a particle will be active in the simulation
	unsigned int emissionRate; // Particles emitted per second of simulation
	float gravity; // Gravitational force acting on particles, eg :-9.81f (Earth)
	Material* material; // The material used to display the particles
};

class ParticleSystem
{
private:
	std::vector<Particle> m_particles;
	std::list<Particle*> m_activeParticles;
	std::list<Particle*> m_inActiveParticles;
	ParticleSystemSettings m_settings;
	Mesh* m_mesh = nullptr;
	VertexConstantBuffer<CB_VS_VertexShader> m_constantBuffer;
public:
	void init(ID3D11Device* device, ID3D11DeviceContext* context, const ParticleSystemSettings& settings);
	void update(float deltaTime);
	void draw(ID3D11DeviceContext* context, const Camera& camera, VertexConstantBuffer<CB_VS_VertexShader>& constantBuffer);
	~ParticleSystem();
};