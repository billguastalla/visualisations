#include "ParticleSet.h"
#include "GeometryTools.h"
#include "Constants.h"
#include <random>

std::vector<glm::vec3> ParticleSet::sampleHelicalTrajectory(float tStart, float tEnd, size_t samples,
	glm::vec3 sinAmp, glm::vec3 sinFreq)
{
	std::vector<glm::vec3> result{};
	float increment{ (tEnd - tStart) / (float)samples };
	for (size_t i{ 0 }; i < samples; ++i)
	{
		float t{ increment * (float)i };
		result.push_back(glm::vec3{ (sinAmp.x * std::sin(sinFreq.x * t)), (sinAmp.y * std::sin(sinFreq.y * t)),(sinFreq.z * std::cos(sinFreq.z * t)) });
	}
	return result;
}

ParticleSet::ParticleSet(const Texture& t)
	:
	m_particles{},
	m_particleShader{ "../Shaders/Particles_Vertex.vs", "../Shaders/Particles_Fragment.fs" },
	m_particleTexture{ t },
	m_particleVAO{ 0u },
	m_particleLifetime{ 2.0f },
	m_maxParticleCount{ 1000 },
	m_seed{ 0u }
{
	unsigned int VBO{ 0u };
	glGenVertexArrays(1, &m_particleVAO);
	glGenBuffers(1, &VBO);
	glBindVertexArray(m_particleVAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(SQUARE_MESH), SQUARE_MESH, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
	glBindVertexArray(0);

}

void ParticleSet::setTexture(const Texture& t)
{
	m_particleTexture = t;
}

void ParticleSet::draw(const Camera& camera)
{
	glBindTexture(GL_TEXTURE_2D, m_particleTexture.id);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);


	sortParticles(camera.m_position);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE);

	m_particleShader.use();

	glm::mat4 projection = glm::perspective(glm::radians(camera.m_zoom), (float)1920 / (float)1080, 0.1f, 100.0f);
	glm::mat4 view = camera.GetViewMatrix();


	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, m_particleTexture.id); // guess
	for (Particle p : m_particles)
	{
		glm::mat4 model{ 1.0 };
		model = glm::translate(model, p.pos);

		m_particleShader.setVec3("cameraRight", camera.m_right);
		m_particleShader.setVec3("cameraUp", camera.m_up);

		m_particleShader.setMat4("projection", projection);
		m_particleShader.setMat4("view", view);

		m_particleShader.setVec3("position", p.pos);
		m_particleShader.setVec4("colour", p.col * p.lifeScale);
		m_particleShader.setFloat("scale", 0.05f * p.scale * p.lifeScale);
		//m_particleShader.setVec3("velocity", p.velocity);
		glBindVertexArray(m_particleVAO);
		glDrawArrays(GL_TRIANGLES, 0, 6);
		glBindVertexArray(0u);
	}
	glBindTexture(GL_TEXTURE0, 0u);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glDisable(GL_BLEND);
}

void ParticleSet::generateParticles(std::vector<glm::vec3> path, const ParticleEmissionSettings& settings)
{
	assert(path.size() > 2);

	std::uniform_real_distribution<float> distDirection{ -1.0, 1.0 }; // used to generate random direction in orthogonal plane to path

	std::random_device rd{};
	std::mt19937 gen{ rd() };
	gen.seed(m_seed++);

	// Distribution for random direction in plane orthogonal to path
	for (size_t i = 0; i < path.size() - 1u; ++i)
	{
		glm::vec3& pNext = path[i + 1];
		glm::vec3& pCurrent = path[i];
		glm::vec3 pathDirection{ pNext - pCurrent };
		std::pair<glm::vec3, glm::vec3> normalBasis{ Geometry::normalBasis(pathDirection) }; // build basis for normal plane to path
		glm::vec3 particleDirection{}; // build random direction in plane
		switch (settings.m_emissionDirection)
		{	// TODO: Remove switch somehow
		case ParticleEmissionSettings::EmissionDirection::OrthogonalPlane:
			particleDirection = glm::normalize((normalBasis.first * distDirection(gen)) + (normalBasis.second * distDirection(gen)));
			break;
		case ParticleEmissionSettings::EmissionDirection::Track:
			particleDirection = glm::normalize(pathDirection);
			break;
		case ParticleEmissionSettings::EmissionDirection::Spherical:
			particleDirection = glm::normalize(glm::vec3{ distDirection(gen),distDirection(gen),distDirection(gen) });
			break;
		default:
			break;
		}

		float meanLength{ settings.m_meanTravelDist * glm::length(pathDirection) };
		std::normal_distribution<float> distScale{ meanLength, meanLength * settings.m_sigmaTravelDist }; // used to distribute velocity magnitude

		Particle part{};
		part.pos = pCurrent;
		part.duration = 0.f;
		part.col = glm::normalize(glm::vec4{ distScale(gen),distScale(gen),distScale(gen),1.f });
		part.lifeScale = 1.f;
		part.scale = settings.m_globalParticleScale;
		part.velocity = particleDirection * distScale(gen) * settings.m_globalVelocity;// * distScale(gen); // direction of particle * gaussian scaling of velocity
		part.velocityDecay = settings.m_baseDecayRate * distScale(gen); // particle velocity decays at gaussian-determined rate

		m_particles.push_back(std::move(part));
	}
}

void ParticleSet::moveParticles(float dt)
{
	for (Particle& p : m_particles)
	{
		p.pos += p.velocity * dt;
		float decay{ std::exp(-p.velocityDecay * dt) };
		p.velocity *= decay;
		p.duration += dt;
		p.lifeScale = (m_particleLifetime - p.duration) / m_particleLifetime;
	}
}

void ParticleSet::clearParticles()
{
	m_particles.erase(
		std::remove_if(m_particles.begin(), m_particles.end(), [this](Particle& p) { return p.duration > m_particleLifetime; }),
		m_particles.end());

}

void ParticleSet::sortParticles(glm::vec3 cameraPos)
{
	auto distSort = [cameraPos](const Particle& p1, const Particle& p2)
	{
		return glm::length(p1.pos - cameraPos) > glm::length(p2.pos - cameraPos);
	};
	std::sort(m_particles.begin(), m_particles.end(), distSort);
}

