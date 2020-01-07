#pragma once
#include "Visualisation.h"
#include "Camera.h"
#include "Mesh.h"

#include <deque>
#include <complex>

class Buffer;
struct GLFWwindow;
class Shader;

class Visualisation_Fractal : public Visualisation
{
public:
	Visualisation_Fractal();

	void activate() override;
	void deactivate() override;

	void processSamples(const Buffer & buf, unsigned samples) override;
	void renderFrame() override;

	std::string titleString() override { return "Mandelbulb"; };

	void drawInterface() override;
private:
	unsigned int m_cubeVAO, m_lightVAO, m_cubeVBO;
	unsigned int m_lineStripVAO, m_lineStripVBO;

	Shader * m_objectShader;
	Shader * m_lampShader;
	Shader * m_lineStripShader;

	std::vector<float> m_vertices;
	glm::vec3 m_lightPos;
	
	std::vector<glm::vec3> m_cubePositions;
	std::vector<glm::vec3> m_cubeColours;
	std::vector<glm::vec3> m_lineStripData;

	std::vector<glm::vec3> m_mandelbulb;	
	int m_mandelbulbRes = 32;
	float m_mandelbulbN = 8.0f;
	glm::vec3 m_mandelbulbInitialVector = glm::vec3{ 0.0,0.0,0.0 };

	Mesh m_sphere;

};