#pragma once
#include "Particle.h"
#include "../Material.h"
#include "../Mesh.h"
#include "../Camera.h"
#include <vector>
#include <list>
#include <random>

struct SpawnOffsetVariance
{
	float minX, maxX;
	float minY, maxY;
	float minZ, maxZ;
};

struct ScaleVariance
{
	float minScale, maxScale;
};

struct LifespanVariance
{
	float minLifeTime, maxLifeTime;
};

struct EmissionInfo
{
	unsigned int emissionAmount;// Particles emitted per unit of simulation
	float emissionFrequency; // Time gap in seconds between each emission
};

struct ParticleSystemSettings
{
public:
	unsigned int maxParticles; // Total number of particles involved
	LifespanVariance lifeSpan; // Time in seconds that a particle will be active in the simulation
	EmissionInfo emissionInfo; // Info amount how the particles will be spawned
	SpawnOffsetVariance spawnOffset; // The possible locations from the origin where particles can be spawned
	ScaleVariance scale; // The scale of the particles being spawned
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
	std::default_random_engine m_randomEngine;
public:
	void init(ID3D11Device* device, ID3D11DeviceContext* context, const ParticleSystemSettings& settings);
	void update(float deltaTime);
	void draw(ID3D11DeviceContext* context, const Camera& camera, VertexConstantBuffer<CB_VS_VertexShader>& constantBuffer);
	Transform transform;
	~ParticleSystem();
};