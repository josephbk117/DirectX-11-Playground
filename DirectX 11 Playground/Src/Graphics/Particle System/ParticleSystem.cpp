#include "ParticleSystem.h"
#include "../Primitive3DModels.h"

void ParticleSystem::init(ID3D11Device* device, ID3D11DeviceContext* context, const ParticleSystemSettings& settings)
{
	this->m_settings = settings;
	this->m_particles.reserve(m_settings.maxParticles);

	this->m_constantBuffer.init(device, context);

	auto vertices = Primitive3DModels::QUAD.vertices;
	auto indices = Primitive3DModels::QUAD.indices;
	m_mesh = new Mesh(device, context, vertices, indices);

	for (unsigned int index = 0; index < m_settings.maxParticles; index++)
	{
		m_particles.emplace_back(XMFLOAT3{ 4, 1, 0 },
			XMFLOAT3{ 2.0f * index / m_settings.maxParticles * 1.0f, 6.0f * index / m_settings.maxParticles * 1.0f, 2.0f * index / m_settings.maxParticles * 1.0f },
			m_settings.gravity, m_settings.maxLifetimeForParticle, 1.0f, sinf(index * 10.0f / 3.142f) * 0.5f + 0.5f);
		if (index < m_settings.emissionRate)
		{
			m_particles.at(index).setActiveState(true);
			m_activeParticles.push_back(&m_particles.at(index));
		}
		else
		{
			m_inActiveParticles.push_back(&m_particles.at(index));
		}
	}
}

void ParticleSystem::update(float deltaTime)
{
	static float timeSinceEmission = 0;
	if (timeSinceEmission > 1.0f)
	{
		timeSinceEmission = 0;
		unsigned int numberOfParticlesToActivate = std::min(m_settings.emissionRate, static_cast<unsigned int>(m_inActiveParticles.size()));
		unsigned int counter = 0;
		std::vector<Particle*> particlesToRemoveFromInActiveParticles;
		for (Particle* particle : m_inActiveParticles)
		{
			if (counter >= numberOfParticlesToActivate)
				break;

			particle->setActiveState(true);
			m_activeParticles.push_back(particle);
			particlesToRemoveFromInActiveParticles.push_back(particle);
			counter++;
		}

		for (Particle* particle : particlesToRemoveFromInActiveParticles)
			m_inActiveParticles.remove(particle);
	}

	std::vector<Particle*> particlesToRemoveFromActiveParticles;
	for (Particle* particle : m_activeParticles)
	{
		if (!particle->update(deltaTime))
		{
			particlesToRemoveFromActiveParticles.push_back(particle);
			particle->setActiveState(false);
			particle->resetStates(XMFLOAT3{ 4, 1, 0 }, XMFLOAT3{ 0,1, -1 }, m_settings.gravity, m_settings.maxLifetimeForParticle, 0, 0.25f);
			m_inActiveParticles.push_back(particle);
		}

	}

	for (Particle* particle : particlesToRemoveFromActiveParticles)
		m_activeParticles.remove(particle);
	timeSinceEmission += deltaTime;
}

void ParticleSystem::draw(ID3D11DeviceContext* context, const Camera& camera, VertexConstantBuffer<CB_VS_VertexShader>& constantBuffer)
{
	XMMATRIX viewProjectionMatrix = camera.GetMatrix() * camera.GetProjectionMatrix();
	m_settings.material->bind(context);
	for (const Particle& particle : m_particles)
	{
		XMFLOAT3 position = particle.getPosition();
		float scale = particle.getScale();
		float angle = atan2(position.x - camera.GetPositionFloat3().x, position.z - camera.GetPositionFloat3().z) * (180.0 / 3.142f);
		float rotation = (float)angle * 0.0174532925f;
		constantBuffer.data.worldMatrix = DirectX::XMMatrixRotationY(rotation) * DirectX::XMMatrixScaling(scale, scale, scale) * DirectX::XMMatrixTranslation(position.x, position.y, position.z);
		constantBuffer.data.mvpMatrix = constantBuffer.data.worldMatrix * viewProjectionMatrix;

		constantBuffer.applyChanges();
		if (particle.getActiveState())
			m_mesh->draw();
	}
}

ParticleSystem::~ParticleSystem()
{
	delete m_mesh;
}
