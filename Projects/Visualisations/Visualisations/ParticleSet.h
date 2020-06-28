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

class ParticleSet
{
public:
	ParticleSet(const Texture& t = Texture{});
	void setTexture(const Texture & t);

	static std::vector<glm::vec3> sampleHelicalTrajectory(float tStart, float tEnd, size_t samples,
		glm::vec3 sinAmp, glm::vec3 sinFreq);

	void draw(const Camera & camera);


	void generateParticles(std::vector<glm::vec3> path, float meanTravelDist = 1.f, float sigmaFractionTravelDist = 5.f, float baseDecayRate = 1.f, float globalParticleScale = 1.f);
	void moveParticles(float dt = .02f);
	void clearParticles();

private:
	void sortParticles(glm::vec3 cameraPos);


	std::vector<Particle> m_particles;
	Shader m_particleShader;
	Texture m_particleTexture;
	unsigned int m_particleVAO;

	float m_particleLifetime;
	unsigned int m_maxParticleCount;


	long m_seed;
};