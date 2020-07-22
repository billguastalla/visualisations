#pragma once
#include "Visualisation.h"
#include "Camera.h"
#include "Mesh.h"

#include <deque>
#include <complex>
#include <GLM/gtc/quaternion.hpp>

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
	void renderFrame(const Camera& camera, const Timestep& t) override;

	std::string titleString() override { return "Mandelbulb"; };

	void drawInterface() override;

	enum class FractalSet
	{
		BifurcationMap,
		Mandelbulb
	};
	std::string fractalSetString()
	{
		return "Bifurcation Map\0Mandelbulb\0";
	}
private:
	FractalSet m_state;

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

	/* MANDELBULB*/
	std::vector<glm::vec3> m_mandelbulb;	
	int m_mandelbulbRes = 32;
	float m_mandelbulbN = 8.0f;
	glm::vec3 m_mandelbulbInitialVector = glm::vec3{ 0.0,0.0,0.0 };

	Mesh m_sphere;

	/* BIFURCATION */
	int m_bifurDim{ 256 };
	double m_bifurX_n{ 1.0 }, m_bifurRbegin{ -2.0 }, m_bifurREnd{ 4.0 }, m_bifurInitialX{ 0.5 };
	std::deque<float> m_bifur{};
	Mesh m_graph;



	void bifurcation();
	/* Just checking orientation. */
	glm::quat m_rotation;

};