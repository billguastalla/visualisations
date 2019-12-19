#pragma once
#include "Visualisation.h"
#include "Camera.h"

#include <deque>
class Buffer;
struct GLFWwindow;
class Shader;

class Visualisation_PointClouds : public Visualisation
{
public:
	Visualisation_PointClouds();

	/* Set up callbacks, shaders, vertex objects */
	void activate() override;
	/* Free callbacks, shaders, vertex objects */
	void deactivate() override;

	void processSamples(const Buffer & buf, unsigned samples) override;
	void renderFrame() override;

	std::string titleString() override { return "Point Clouds"; };
private:


	unsigned int m_cubeVAO, m_lightVAO, m_cubeVBO;

	Shader * m_objectShader;
	Shader * m_lampShader;

	std::vector<float> m_vertices;

	std::vector<glm::vec3> m_cubePositions;
	std::vector<glm::vec3> m_cubeColours;

	std::deque<double> m_signalPowerHistory;

	glm::vec3 m_lightPos;

	
};