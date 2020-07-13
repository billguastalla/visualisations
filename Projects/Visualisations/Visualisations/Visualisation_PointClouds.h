#pragma once
#include "Visualisation.h"
#include "Mesh.h"
#include "MeshGenerator.h"

class Buffer;
struct GLFWwindow;
class Shader;

class Visualisation_PointClouds : public Visualisation
{
public:
	Visualisation_PointClouds();
	void activate() override;
	void deactivate() override;
	void processSamples(const Buffer & buf, unsigned samples) override;
	void renderFrame(const Camera& camera, const Timestep& t) override;
	std::string titleString() override { return "Point Clouds"; };
	void drawInterface() override;

private:
	Mesh m_cube;
	Shader * m_objectShader;
	Shader * m_lampShader;
	std::vector<glm::vec3> m_cubePositions;
	std::vector<glm::vec3> m_cubeScales;
	std::vector<glm::vec3> m_cubeColours;
	std::deque<double> m_signalPowerHistory;
	glm::vec3 m_lightPos;

	/* Settings: */
	int m_maxShapes; 
	int m_shapesPerFrame; 
	int m_shapeRes; 
	bool m_rescaleShapeOverTime; 
	int m_shapeType; 
	glm::vec3 m_colourDeviation; 
	glm::vec3 m_distanceDeviation; 
};