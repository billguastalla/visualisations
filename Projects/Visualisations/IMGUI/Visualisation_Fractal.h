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

	/* Set up callbacks, shaders, vertex objects */
	void activate() override;
	/* Free callbacks, shaders, vertex objects */
	void deactivate() override;

	void processSamples(const Buffer & buf, unsigned samples) override;
	void renderFrame() override;

	std::string titleString() override { return "Mandelbulb Set"; };
private:
	int inSet(std::complex<long double> z, std::complex<long double> c);
	std::vector<glm::vec3> buildFractalData(std::complex<long double> k);




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

	std::deque<double> m_signalPowerHistory;

	long double m_FCT = 2.85, m_hFCT = m_FCT / 2.0;
	long int m_iterations = 512;
};