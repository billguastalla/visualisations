#include "ParticleSet.h"
#include "GeometryTools.h"
#include "Constants.h"
#include "imgui/imgui.h"
#include <random>

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

		if (pathDirection == glm::vec3{ 0.f })
			pathDirection = glm::vec3{ 1.f,0.f,0.f }; // TODO: Handle this circumstance better (this avoids sigma = 0)

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
		if (meanLength == 0.f)
			meanLength = 0.1f; // TODO: Better edge cases.
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

void ParticleSet::moveParticles(float dt, float particleLifetime)
{
	for (Particle& p : m_particles)
	{
		p.pos += p.velocity * dt;
		float decay{ std::exp(-p.velocityDecay * dt) };
		p.velocity *= decay;
		p.duration += dt;
		p.lifeScale = (particleLifetime - p.duration) / particleLifetime;
	}
}

void ParticleSet::clearParticles(float particleLifetime)
{
	m_particles.erase(
		std::remove_if(m_particles.begin(), m_particles.end(), [this,particleLifetime](Particle& p) { return p.duration > particleLifetime; }),
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

void ParticleEmissionSettings::drawUI()
{
	ImGui::Text("Emission Settings:");
	ImGui::SliderFloat("Travel Distance Mean", &m_meanTravelDist, 1.f, 10.0f);
	ImGui::SliderFloat("Travel Distance Sigma", &m_sigmaTravelDist, 0.03f, 1.0f);
	ImGui::SliderFloat("Velocity Decay Rate Base", &m_baseDecayRate, 0.1f, 5.f);
	ImGui::SliderFloat("Global Particle Scale", &m_globalParticleScale, 0.1f, 100.f);
	ImGui::SliderFloat("Global Velocity", &m_globalVelocity, 0.1f, 10.f);

	
}
