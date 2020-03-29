#include "Visualisation_Fractal.h"

#include <glad\glad.h>
#include <GLFW\glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Shader.h"
#include "Buffer.h"

#include "MeshGenerator.h"

#include "MetaDistribution.h"

/* UI */
#include "imgui/imgui.h"
#include "imgui/examples/imgui_impl_glfw.h"
#include "imgui/examples/imgui_impl_opengl3.h"

#include <iostream>
#include <complex>
#include <vector>

Visualisation_Fractal::Visualisation_Fractal()
	:
	m_lightPos{ 0.0f,0.0f,0.0f },
	m_objectShader{ nullptr },
	m_lampShader{ nullptr },

	m_cubeVAO{ 0 },
	m_lightVAO{ 0 },
	m_cubeVBO{ 0 },
	m_lineStripVAO{ 0 },
	m_lineStripVBO{ 0 },
	m_lineStripShader{ nullptr },

	m_mandelbulb{},
	m_mandelbulbRes{ 32 },
	m_mandelbulbN{ 8 },
	m_mandelbulbInitialVector{ 0.3,0.2,0.5 },

	m_sphere{},

	m_graph{},
	m_state{ FractalSet::Mandelbulb },
	m_bifurDim{ 256 },
	m_bifurX_n{ 1.0 },
	m_bifurRbegin{ -2.0 },
	m_bifurREnd{ 4.0 },
	m_bifurInitialX{ 0.5 },

	m_rotation{0.0,0.0,0.0,0.0}
{
}

void Visualisation_Fractal::activate()
{
	m_objectShader = new Shader{ "../Shaders/Cubes_Vertex.vs", "../Shaders/Cubes_ObjectFragment.fs" };
	m_lineStripShader = new Shader{ "../Shaders/Fractal_Linestrip_Vertex.vs", "../Shaders/Fractal_Linestrip_Fragment.fs" };
	m_lampShader = new Shader{ "../Shaders/Cubes_Vertex.vs", "../Shaders/Cubes_LampFragment.fs" };

	m_mandelbulb = MeshGenerator::generateMandelbulb(m_mandelbulbRes, m_mandelbulbN, m_mandelbulbInitialVector);
	MeshGenerator::generateSphere(4, m_sphere);
	m_sphere.scale(glm::vec3{ 0.004, 0.004, 0.004 });

	bifurcation();

	m_active = true;
}

void Visualisation_Fractal::deactivate()
{
	glDeleteVertexArrays(1, &m_cubeVAO);
	glDeleteVertexArrays(1, &m_lightVAO);
	glDeleteBuffers(1, &m_cubeVBO);

	m_cubePositions.clear();
	m_vertices.clear();

	delete m_lampShader;
	m_lampShader = nullptr;
	delete m_objectShader;
	m_objectShader = nullptr;
	delete m_lineStripShader;
	m_lineStripShader = nullptr;
}

void Visualisation_Fractal::processSamples(const Buffer& buf, unsigned samples)
{
}

void Visualisation_Fractal::renderFrame()
{
	float currentFrame = glfwGetTime();
	m_deltaTime = currentFrame - m_lastFrame;
	m_lastFrame = currentFrame;

	m_objectShader->use();
	m_objectShader->setVec3("lightColour", glm::vec3{ 1.0f,0.5f,0.31f });
	m_objectShader->setVec3("objectColour", glm::vec3{ 0.6f,0.3f,0.21f });
	glm::mat4 projection = glm::perspective(glm::radians(m_camera.m_zoom), (float)1920 / (float)1080, 0.1f, 100.0f);
	m_objectShader->setMat4("projection", projection);

	glm::mat4 view = m_camera.GetViewMatrix();
	m_objectShader->setMat4("view", view);
	glm::mat4 lightModel{ 1.0f };
	m_lightPos = glm::vec3(5 * sin(glfwGetTime()), 5 * cos(glfwGetTime()), 5 * sin(glfwGetTime()));
	lightModel = glm::translate(lightModel, m_lightPos);
	lightModel = glm::scale(lightModel, glm::vec3{ 0.2f });
	m_objectShader->setVec3("lightPos", m_lightPos);
	m_objectShader->setVec3("viewPos", m_camera.m_position);

	switch (m_state)
	{
	case FractalSet::BifurcationMap:
	{
		glm::mat4 mod{ m_rotation };
		//mod = glm::rotate(mod, 3.141f, glm::vec3{ 0,1,0 });
		//mod = glm::rotate(mod, 3.141f, glm::vec3{ 1,0,0 });
		//mod = m_rotation * mod;
		mod = glm::translate(mod, glm::vec3{ 0.0f,3.0f,0.0f });
		m_objectShader->setMat4("model", mod);
		m_graph.draw(m_objectShader);
		break;
	}
	case FractalSet::Mandelbulb:
	default:
	{
		for (auto i = m_mandelbulb.begin(); i != m_mandelbulb.end(); ++i)
		{
			glm::mat4 mod{ 1.0 };
			mod = glm::translate(mod, *i);
			m_objectShader->setMat4("model", mod);
			m_sphere.draw(m_objectShader);
		}
		break;
	}
	}


	m_lampShader->use();
	m_lampShader->setMat4("projection", projection);
	m_lampShader->setMat4("view", view);
	m_lampShader->setMat4("model", lightModel);
}

void Visualisation_Fractal::drawInterface()
{

	int setState = (int)m_state;
	bool s = (setState ==  (int)FractalSet::BifurcationMap);
	ImGui::Checkbox("Bifur", &s);
	if (s)
		setState = (int)FractalSet::BifurcationMap;
	else
		setState = (int)FractalSet::Mandelbulb;

	switch ((FractalSet)setState)
	{
	case FractalSet::BifurcationMap:
	{
		int dim{ m_bifurDim };
		float rBegin{ (float)m_bifurRbegin }, rEnd{ (float)m_bifurREnd }, initialX{ (float)m_bifurInitialX };
		ImGui::SliderFloat("Initial X", &initialX, 0.0f, 5.0f);
		ImGui::SliderFloat("R Begin", &rBegin, -30.0f, 5.0f);
		ImGui::SliderFloat("R End", &rEnd, 0.0f, 30.0f);
		ImGui::SliderInt("Width", &dim, 16, 512);

		ImGui::SliderFloat3("Rotation Axis", &m_rotation[0], 0.0f, 1.0f);
		ImGui::SliderFloat("Rotation Angle", &m_rotation[3], 0.0f, 360.0f);

		if (
			rBegin != (float)m_bifurRbegin ||
			rEnd != (float)m_bifurREnd ||
			initialX != (float)m_bifurInitialX ||
			dim != m_bifurDim
			)
		{
			m_bifurRbegin = (double)rBegin;
			m_bifurREnd = (double)rEnd;
			m_bifurInitialX = (double)initialX;
			m_bifurDim = dim;
			bifurcation();
		}
		break;
	}
	case FractalSet::Mandelbulb:
	{
		float n = m_mandelbulbN;
		int res = m_mandelbulbRes;
		glm::vec3 ini = m_mandelbulbInitialVector;

		ImGui::SliderFloat("N", &m_mandelbulbN, 1.0, 16.0);
		ImGui::SliderInt("Resolution", &m_mandelbulbRes, 1, 800000);
		ImGui::SliderFloat3("Initial", (float*)&m_mandelbulbInitialVector, 0.0, 1.0);

		if (n != m_mandelbulbN || res != m_mandelbulbRes || ini != m_mandelbulbInitialVector)
			m_mandelbulb = MeshGenerator::generateMandelbulb(m_mandelbulbRes, m_mandelbulbN, m_mandelbulbInitialVector);
		break;
	}
	default:
		break;
	}

	m_state = (FractalSet)setState;
}

void Visualisation_Fractal::bifurcation()
{
	/* Bifurcation diagram: */
	/* x_n = r * x_n-1 (1 - x_n-1) */
	// Graph: x-axis: r, y-axis: x_n in lim n -> inf. 
	/* R from -2 to 4 */
	m_bifur.clear();
	double bifurIncrement = (m_bifurREnd - m_bifurRbegin) / (double)m_bifurDim;
	for (double r = m_bifurRbegin; r < m_bifurREnd; r += bifurIncrement)
	{
		m_bifurX_n = m_bifurInitialX;
		for (int n= 0; n < m_bifurDim; ++n)
			m_bifur.push_back((float)(m_bifurX_n = (r * m_bifurX_n * (1 - m_bifurX_n))));
	}
	MeshGenerator::generateGraph(m_bifurDim - 1, m_bifurDim - 1, m_graph, m_bifur);

	m_graph.scale(glm::vec3{(m_bifurREnd - m_bifurRbegin),1.0f,1.0f});
}



//// Taken from: https://rosettacode.org/wiki/Julia_set#C.2B.2B
//int Visualisation_Fractal::inSet(std::complex<long double> z, std::complex<long double> c)
//{
//	for (int ec = 0; ec < m_iterations; ec++) {
//		z = z * z;
//		z = z + c;
//		long double dist{ (z.imag() * z.imag()) + (z.real() * z.real()) };
//		if (dist > 3.0l) return(ec);
//	}
//	return 0;
//}
//
//std::vector<glm::vec3> Visualisation_Fractal::buildFractalData(std::complex<long double> k)
//{
//	std::vector<glm::vec3> result;
//	int res, pos;
//	int BMP_SIZE = 100;
//	std::complex<long double> c, factor(m_FCT / BMP_SIZE, m_FCT / BMP_SIZE);
//	for (int y = 0; y < BMP_SIZE; y++)
//	{
//		pos = y * BMP_SIZE;
//		c.imag((factor.imag() * y) + -m_hFCT);
//		for (int x = 0; x < BMP_SIZE; x++)
//		{
//			glm::vec3 colourVector{ 0.0f,0.0f,0.0f };
//			glm::vec3 positionVector{ (2.0 * (double)x / (double)BMP_SIZE) - 1.0, (2.0 * (double)y / (double)BMP_SIZE) - 1.0, 0.0f };
//
//			c.real(factor.real() * x + -m_hFCT);
//			res = inSet(c, k);
//			if (res) {
//				int n_res = res % 255;
//				if (res < (m_iterations >> 1))
//				{
//					res = RGB(n_res << 2, n_res << 3, n_res << 4);
//					colourVector.r = (double)(n_res << 2) / (double)255;
//					colourVector.g = (double)(n_res << 3) / (double)255;
//					colourVector.b = (double)(n_res << 4) / (double)255;
//				}
//				else
//				{
//					res = RGB(n_res << 4, n_res << 2, n_res << 5);
//					colourVector.r = (double)(n_res << 4) / (double)255;
//					colourVector.g = (double)(n_res << 2) / (double)255;
//					colourVector.b = (double)(n_res << 5) / (double)255;
//				}
//			}
//			/* Two parts: Position & Colour information. */
//			result.push_back(positionVector);
//			result.push_back(colourVector);
//		}
//	}
//	return result;
//}








///////// PREVIOUS MANDELBULB EXPERMIENTS

	///* Mandelbulb is this: */
	////{x, y, z}n = rn{ cos(θ)cos(φ),sin(θ)cos(φ),-sin(φ) }
	////r = sqrt(x2 + y2 + z2), θ = n atan2(y, x), φ = n sin - 1(z / r)
	//auto theta = [](double x, double y, double z) {
	//	return std::atan(sqrt((x*x) + (y*y)) / z);
	//};
	//auto phi = [](double x, double y) {
	//	return std::atan(y / x);
	//};
	//auto radial = [](double x, double y, double z) {
	//	return sqrt((x*x) + (y*y) + (z*z));
	//};
	//auto mandelBulbIter = [radial, phi, theta](glm::dvec3 in, double n) {
	//	glm::dvec3 out{ 0.0 };
	//	double r = radial(in.x, in.y, in.z);
	//	double th = theta(in.x, in.y, in.z);
	//	double ph = phi(in.x, in.y);
	//	out.x = r * (sin(n * th) * cos(n * ph));
	//	out.y = r * (sin(n * th) * sin(n * ph));
	//	out.z = r * cos(n * ph);
	//	return out;
	//};
	//auto buildSequence = [mandelBulbIter](glm::dvec3 initial, double n, int iterations) {
	//	/* Iterations includes initial value. */
	//	std::vector<glm::dvec3> set;
	//	glm::dvec3 current{ initial };
	//	set.push_back(initial);
	//	for (int i = 0; i < (iterations - 1); ++i)
	//		set.push_back(current = mandelBulbIter(current, n));
	//	return set;
	//};
	//auto normaliseSet = [](std::vector<glm::dvec3> & set, const glm::vec3 & scale = glm::vec3{ 1.0 }) {
	//	glm::dvec3 max{ 0.0 };
	//	for (std::vector<glm::dvec3>::iterator i = set.begin(); i != set.end(); ++i)
	//	{
	//		if (max.x < std::abs(i->x))
	//			max.x = std::abs(i->x);
	//		if (max.y < std::abs(i->y))
	//			max.y = std::abs(i->y);
	//		if (max.z < std::abs(i->z))
	//			max.z = std::abs(i->z);
	//	}
	//	for (std::vector<glm::dvec3>::iterator i = set.begin(); i != set.end(); ++i)
	//	{
	//		*i /= max;
	//		*i *= scale;
	//	}
	//};
	//auto doubleToFloat = [](std::vector<glm::dvec3> vec) {
	//	std::vector<glm::vec3> o{};
	//	for (std::vector<glm::dvec3>::iterator i = vec.begin(); i != vec.end(); ++i)
	//		o.push_back(glm::vec3{i->x,i->y,i->z});
	//	return o;
	//};

	///* Tick cycle from 1 to 21 every 3 seconds */
	//int nTicker{ ((int)std::floor(glfwGetTime()) % 20) + 1 };

	///* Tick cycle from 1 - 5 every 11 seconds*/
	//int modeTicker{ ((int)std::floor(0.1 * glfwGetTime()) % 6) + 1 };
	//glm::dvec3 initialInput{ 0.0f };
	//switch (modeTicker)
	//{
	//case 1:
	//	initialInput = glm::dvec3{ sin(0.00001 * glfwGetTime()),cos(0.00001 * glfwGetTime()), tan(0.00001 * glfwGetTime()) };
	//	break;
	//case 2:
	//	initialInput = glm::dvec3{ sin(0.00001 * glfwGetTime()),cos(0.00001 * glfwGetTime()), tan(glfwGetTime()) };
	//	break;
	//case 3:
	//	initialInput = glm::dvec3{ tan(glfwGetTime()),cos(glfwGetTime()), tan(0.00001 * glfwGetTime()) };
	//	break;
	//case 4:
	//	initialInput = glm::dvec3{ sin(glfwGetTime()),sin(glfwGetTime()), sin(glfwGetTime()) };
	//	break;
	//case 5:
	//	initialInput = glm::dvec3{ tan(glfwGetTime()),tan(glfwGetTime()), tan(glfwGetTime()) };
	//	break;
	//case 6:
	//	initialInput = glm::dvec3{ 0.0f };
	//	break;
	//default:
	//	break;
	//}


	//std::vector<glm::dvec3> seq = buildSequence(initialInput, nTicker, 1000);
	////normaliseSet(seq, glm::vec3{ max });
	//m_cubePositions.clear();
	//m_cubeColours.clear();
	//m_cubePositions = doubleToFloat(seq);
	//m_cubeColours.resize(m_cubePositions.size());
	//for (size_t i = 0; i < m_cubeColours.size(); ++i)
	//	m_cubeColours[i] = glm::vec3{ sin(i * 0.06 * glfwGetTime()),cos(i * 0.12 * glfwGetTime()),sin(i * 0.18 * glfwGetTime()) };
	//