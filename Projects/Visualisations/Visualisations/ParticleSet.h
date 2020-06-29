#pragma once
#include "Texture.h"
#include "Camera.h"
#include "Shader.h"
#include <vector>
/*
	Particle trajectories:
		-> Particles need to be given:
			(a) a path to walk through
			(b) a distribution to be generated from
	Particle rendering:
		-> Read a texture file
		-> Particle lifetime
		-> Particle speed
		-> Particle colour

	Particle Shader:
		-> Takes colour, texture, and position.

	Intended design restrictions:
		-> One Texture per particle set.
		-> The line that generates particles can be a curve, but
			particles that are emitted move along a straight line.
*/

struct Particle
{
	glm::vec3 pos; // Position
	glm::vec4 col; // Colour
	float lifeScale; // Fraction of life alive for
	float duration; // Duration alive for
	glm::vec3 velocity; // Velocity
	float velocityDecay; // Velocity decrease over time
	float scale; // Scale
};

constexpr char emissionOptions[] = "Track\0Orthogonal Plane\0Spherical\0";

struct ParticleEmissionSettings
{
	ParticleEmissionSettings()
		:
		m_emissionDirection{ EmissionDirection::OrthogonalPlane },
		m_meanTravelDist{ 1.f },
		m_sigmaTravelDist{ 1.f },
		m_baseDecayRate{ 1.f },
		m_globalParticleScale{ 1.f },
		m_globalVelocity{ 1.f }
	{}

	enum class EmissionDirection
	{
		Track,				// emit particles in a track, in the direction of the sequence of coordinates
		OrthogonalPlane,	// emit particles in an orthogonal plane to the direction of the sequence
		Spherical			// emit particles spherically, 
	} m_emissionDirection;
	
	float m_meanTravelDist;
	float m_sigmaTravelDist;
	float m_baseDecayRate;
	float m_globalParticleScale;
	float m_globalVelocity;
};


class ParticleSet
{
public:
	ParticleSet(const Texture& t = Texture{});

	void draw(const Camera& camera);

	void setTexture(const Texture& t);
	static std::vector<glm::vec3> sampleHelicalTrajectory(float tStart, float tEnd, size_t samples, glm::vec3 sinAmp, glm::vec3 sinFreq);
	void generateParticles(std::vector<glm::vec3> path, const ParticleEmissionSettings& settings); // add particles to the list
	void moveParticles(float dt = .02f); // update the velocity of the particles
	void clearParticles(); // 

private:
	void sortParticles(glm::vec3 cameraPos); // reorder particles in order of distance from the camera, to allow alpha blending.

	Shader m_particleShader;
	Texture m_particleTexture;
	unsigned int m_particleVAO;

	std::vector<Particle> m_particles;
	float m_particleLifetime;			// in seconds, how long before particle should be deleted
	unsigned int m_maxParticleCount;	// max number of particles in set
	long m_seed;						// 
};

