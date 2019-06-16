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

	std::uniform_real_distribution<float> distributer(-1.0f, 1.0f);
	std::uniform_real_distribution<float> scaleDistributer(m_settings.scale.minScale, m_settings.scale.maxScale);
	std::uniform_real_distribution<float> lifeTimeDistributer(m_settings.lifeSpan.minLifeTime, m_settings.lifeSpan.maxLifeTime);

	std::uniform_real_distribution<float> xAxisOffsetDistributer(m_settings.spawnOffset.minX, m_settings.spawnOffset.maxX);
	std::uniform_real_distribution<float> yAxisOffsetDistributer(m_settings.spawnOffset.minY, m_settings.spawnOffset.maxY);
	std::uniform_real_distribution<float> zAxisOffsetDistributer(m_settings.spawnOffset.minZ, m_settings.spawnOffset.maxZ);

	for (unsigned int index = 0; index < m_settings.maxParticles; index++)
	{
		XMFLOAT3 velocityDir;

		velocityDir.x = distributer(m_randomEngine);
		velocityDir.y = 1;
		velocityDir.z = distributer(m_randomEngine);

		XMFLOAT3 pos = transform.GetPositionFloat3();
		pos.x += xAxisOffsetDistributer(m_randomEngine);
		pos.y += yAxisOffsetDistributer(m_randomEngine);
		pos.z += zAxisOffsetDistributer(m_randomEngine);

		m_particles.emplace_back(pos,
			velocityDir,
			m_settings.gravity, lifeTimeDistributer(m_randomEngine), 1.0f, scaleDistributer(m_randomEngine));
		if (index < m_settings.emissionInfo.emissionAmount)
		{
			m_particles.at(index).setActiveState(true);
			m_activeParticles.push_back(&m_particles.at(index));
		}
		else
			m_inActiveParticles.push_back(&m_particles.at(index));
	}
}

void ParticleSystem::update(float deltaTime)
{
	static float timeSinceEmission = 0;
	if (timeSinceEmission > m_settings.emissionInfo.emissionFrequency)
	{
		timeSinceEmission = 0;
		unsigned int numberOfParticlesToActivate = std::min(m_settings.emissionInfo.emissionAmount, static_cast<unsigned int>(m_inActiveParticles.size()));
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
	std::uniform_real_distribution<float> distributer(-1.0f, 1.0f);
	std::uniform_real_distribution<float> scaleDistributer(m_settings.scale.minScale, m_settings.scale.maxScale);
	std::uniform_real_distribution<float> lifeTimeDistributer(m_settings.lifeSpan.minLifeTime, m_settings.lifeSpan.maxLifeTime);

	std::uniform_real_distribution<float> xAxisOffsetDistributer(m_settings.spawnOffset.minX, m_settings.spawnOffset.maxX);
	std::uniform_real_distribution<float> yAxisOffsetDistributer(m_settings.spawnOffset.minY, m_settings.spawnOffset.maxY);
	std::uniform_real_distribution<float> zAxisOffsetDistributer(m_settings.spawnOffset.minZ, m_settings.spawnOffset.maxZ);

	std::vector<Particle*> particlesToRemoveFromActiveParticles;
	for (Particle* particle : m_activeParticles)
	{
		if (!particle->update(deltaTime))
		{
			XMFLOAT3 velocityDir;

			velocityDir.x = distributer(m_randomEngine);
			velocityDir.y = 1;
			velocityDir.z = distributer(m_randomEngine);

			XMFLOAT3 pos = transform.GetPositionFloat3();
			pos.x += xAxisOffsetDistributer(m_randomEngine);
			pos.y += yAxisOffsetDistributer(m_randomEngine);
			pos.z += zAxisOffsetDistributer(m_randomEngine);

			particlesToRemoveFromActiveParticles.push_back(particle);
			particle->setActiveState(false);
			particle->resetStates(pos, velocityDir, m_settings.gravity, lifeTimeDistributer(m_randomEngine), 0, scaleDistributer(m_randomEngine));
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
