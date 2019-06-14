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
		m_particles.emplace_back(XMFLOAT3{ index * 1.0f, 0, 0 }, 
			XMFLOAT3{ 2.0f * index/m_settings.maxParticles*1.0f, 6.0f * index / m_settings.maxParticles * 1.0f, 2.0f * index / m_settings.maxParticles * 1.0f }, 
			m_settings.gravity, m_settings.maxLifetimeForParticle, 1.0f, sinf(index * 10.0f/3.142f) * 0.5f + 0.5f);
	}
}

void ParticleSystem::start()
{
}

void ParticleSystem::update(float deltaTime)
{
	for (Particle& particle : m_particles)
		particle.update(deltaTime);
}

void ParticleSystem::draw(ID3D11DeviceContext* context, const XMMATRIX& viewProjectionMatrix, VertexConstantBuffer<CB_VS_VertexShader>& constantBuffer)
{
	m_settings.material->bind(context);
	for (const Particle& particle : m_particles)
	{
		XMFLOAT3 position = particle.getPosition();
		float scale = particle.getScale();
		constantBuffer.data.mvpMatrix = DirectX::XMMatrixScaling(scale, scale, scale) * DirectX::XMMatrixTranslation(position.x, position.y, position.z) * viewProjectionMatrix;
		constantBuffer.data.worldMatrix = DirectX::XMMatrixScaling(scale, scale, scale) * DirectX::XMMatrixTranslation(position.x, position.y, position.z);
		constantBuffer.applyChanges();
		m_mesh->draw();
	}
}

void ParticleSystem::stop()
{
}

ParticleSystem::~ParticleSystem()
{
	delete m_mesh;
}
