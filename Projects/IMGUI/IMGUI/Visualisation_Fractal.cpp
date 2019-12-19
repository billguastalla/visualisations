#include "Visualisation_Fractal.h"

#include <glad\glad.h>
#include <GLFW\glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Shader.h"
#include "Buffer.h"

#include "MetaDistribution.h"

#include <iostream>
#include <complex>
#include <vector>

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow *window);

Visualisation_Fractal::Visualisation_Fractal()
	:
	m_lightPos{ 0.0f,0.0f,0.0f },
	m_objectShader{ nullptr },
	m_lampShader{ nullptr }
{
}

void Visualisation_Fractal::activate()
{
	// build and compile our shader program
	// ------------------------------------
	m_objectShader = new Shader{ "../Shaders/Cubes_Vertex.vs", "../Shaders/Cubes_ObjectFragment.fs" };
	m_lineStripShader = new Shader{ "../Shaders/Fractal_Linestrip_Vertex.vs", "../Shaders/Fractal_Linestrip_Fragment.fs" };
	m_lampShader = new Shader{ "../Shaders/Cubes_Vertex.vs", "../Shaders/Cubes_LampFragment.fs" };

	// set up vertex data(and buffer(s)) and configure vertex attributes
	// ------------------------------------------------------------------

	m_lightPos = { 0.0,0.0,-8.0 };

	m_vertices = {
		-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
		0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
		0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
		0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
		-0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
		-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,

		-0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
		0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
		0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
		0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
		-0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
		-0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,

		-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
		-0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
		-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
		-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
		-0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
		-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,

		0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
		0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
		0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
		0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
		0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
		0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,

		-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
		0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
		0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
		0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
		-0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,

		-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
		0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
		0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
		0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
		-0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
		-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f
	};
	// world space positions of our cubes
	m_cubePositions = {
		glm::vec3(2.0f,  5.0f, -15.0f),
		glm::vec3(-1.5f, -2.2f, -2.5f),
		glm::vec3(-3.8f, -2.0f, -12.3f),
		glm::vec3(2.4f, -0.4f, -3.5f),
		glm::vec3(-1.7f,  3.0f, -7.5f),
		glm::vec3(1.3f, -2.0f, -2.5f),
		glm::vec3(1.5f,  2.0f, -2.5f),
		glm::vec3(1.5f,  0.2f, -1.5f),
		glm::vec3(-1.3f,  1.0f, -1.5f)
	};
	m_cubeColours = {
		glm::vec3(1.0f,0.0f,0.0f),
		glm::vec3(0.0f,1.0f,0.0f),
		glm::vec3(0.0f,0.0f,1.0f),
		glm::vec3(1.0f,0.0f,0.0f),
		glm::vec3(0.0f,1.0f,0.0f),
		glm::vec3(0.0f,0.0f,1.0f),
		glm::vec3(1.0f,0.0f,0.0f),
		glm::vec3(0.0f,1.0f,0.0f),
		glm::vec3(0.0f,0.0f,1.0f)
	};

	glGenVertexArrays(1, &m_cubeVAO);
	glGenBuffers(1, &m_cubeVBO);
	glBindVertexArray(m_cubeVAO);
	glBindBuffer(GL_ARRAY_BUFFER, m_cubeVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * m_vertices.size(), &m_vertices[0], GL_STATIC_DRAW);
	// position attribuet
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	/* LINE STRIP */
	glGenVertexArrays(1, &m_lineStripVAO);
	glGenBuffers(1, &m_lineStripVBO);
	glBindVertexArray(m_lineStripVAO);
	glBindBuffer(GL_ARRAY_BUFFER, m_lineStripVBO);
	std::vector<glm::vec3> initialLine{
		glm::vec3{-0.5f, -0.5f, 0.0f}, glm::vec3{1.0f, 0.0f, 0.0f},
		glm::vec3{ 0.5f,0.5f,0.0f}, glm::vec3{ 0.0f,0.0f,1.0f}
	};
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * initialLine.size(), &initialLine[0], GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	//glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
	//glEnableVertexAttribArray(1);


	glGenVertexArrays(1, &m_lightVAO);
	glBindVertexArray(m_lightVAO);
	glBindBuffer(GL_ARRAY_BUFFER, m_cubeVBO);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
}

void Visualisation_Fractal::deactivate()
{
	// optional: de-allocate all resources once they've outlived their purpose:
	// ------------------------------------------------------------------------
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

void Visualisation_Fractal::processSamples(const Buffer & buf, unsigned samples)
{
	std::complex<long double> c;
	long double factor = m_FCT / 100.0;
	c.imag((factor * 184) + -1.4 + cos(glfwGetTime()));
	c.real((factor * 307) + -2.0 + sin(glfwGetTime()));

	/* Mandelbulb is this: */
	//{x, y, z}n = rn{ cos(θ)cos(φ),sin(θ)cos(φ),-sin(φ) }
	//r = sqrt(x2 + y2 + z2), θ = n atan2(y, x), φ = n sin - 1(z / r)
	auto theta = [](double x, double y, double z) {
		return std::atan(sqrt((x*x) + (y*y)) / z);
	};
	auto phi = [](double x, double y) {
		return std::atan(y / x);
	};
	auto radial = [](double x, double y, double z) {
		return sqrt((x*x) + (y*y) + (z*z));
	};
	auto mandelBulbIter = [radial, phi, theta](glm::dvec3 in, double n) {
		glm::dvec3 out{ 0.0 };
		double r = radial(in.x, in.y, in.z);
		double th = theta(in.x, in.y, in.z);
		double ph = phi(in.x, in.y);
		out.x = r * (sin(n * th) * cos(n * ph));
		out.y = r * (sin(n * th) * sin(n * ph));
		out.z = r * cos(n * ph);
		return out;
	};
	auto buildSequence = [mandelBulbIter](glm::dvec3 initial, double n, int iterations) {
		/* Iterations includes initial value. */
		std::vector<glm::dvec3> set;
		glm::dvec3 current{ initial };
		set.push_back(initial);
		for (int i = 0; i < (iterations - 1); ++i)
			set.push_back(current = mandelBulbIter(current, n));
		return set;
	};
	auto normaliseSet = [](std::vector<glm::dvec3> & set, const glm::vec3 & scale = glm::vec3{ 1.0 }) {
		glm::dvec3 max{ 0.0 };
		for (std::vector<glm::dvec3>::iterator i = set.begin(); i != set.end(); ++i)
		{
			if (max.x < std::abs(i->x))
				max.x = std::abs(i->x);
			if (max.y < std::abs(i->y))
				max.y = std::abs(i->y);
			if (max.z < std::abs(i->z))
				max.z = std::abs(i->z);
		}
		for (std::vector<glm::dvec3>::iterator i = set.begin(); i != set.end(); ++i)
		{
			*i /= max;
			*i *= scale;
		}
	};
	auto doubleToFloat = [](std::vector<glm::dvec3> vec) {
		std::vector<glm::vec3> o{};
		for (std::vector<glm::dvec3>::iterator i = vec.begin(); i != vec.end(); ++i)
			o.push_back(glm::vec3{i->x,i->y,i->z});
		return o;
	};



	//std::map<int, std::deque<float>> & data = buf.data();
	//std::deque<float> & ch = data.at(0);
	//double signalPowerVal{ 0.0 };
	//for (int i = 0; i < ch.size(); ++i)
	//	signalPowerVal += (ch[i] * ch[i]);
	//signalPowerVal /= (double)ch.size();

	//m_signalPowerHistory.push_back(signalPowerVal);
	//if (m_signalPowerHistory.size() > 10)
	//	m_signalPowerHistory.pop_front();

	//double signalPowerAverage{ 0.0 };
	//for (size_t i = 0; i < m_signalPowerHistory.size(); ++i)
	//	signalPowerAverage += m_signalPowerHistory[i];
	//signalPowerAverage /= (double)m_signalPowerHistory.size();
	//float max = buf.amplitude_peak();

	/* Tick cycle from 1 to 21 every 3 seconds */
	int nTicker{ ((int)std::floor(glfwGetTime()) % 20) + 1 };

	/* Tick cycle from 1 - 5 every 11 seconds*/
	int modeTicker{ ((int)std::floor(0.1 * glfwGetTime()) % 6) + 1 };
	glm::dvec3 initialInput{ 0.0f };
	switch (modeTicker)
	{
	case 1:
		initialInput = glm::dvec3{ sin(0.00001 * glfwGetTime()),cos(0.00001 * glfwGetTime()), tan(0.00001 * glfwGetTime()) };
		break;
	case 2:
		initialInput = glm::dvec3{ sin(0.00001 * glfwGetTime()),cos(0.00001 * glfwGetTime()), tan(glfwGetTime()) };
		break;
	case 3:
		initialInput = glm::dvec3{ tan(glfwGetTime()),cos(glfwGetTime()), tan(0.00001 * glfwGetTime()) };
		break;
	case 4:
		initialInput = glm::dvec3{ sin(glfwGetTime()),sin(glfwGetTime()), sin(glfwGetTime()) };
		break;
	case 5:
		initialInput = glm::dvec3{ tan(glfwGetTime()),tan(glfwGetTime()), tan(glfwGetTime()) };
		break;
	case 6:
		initialInput = glm::dvec3{ 0.0f };
		break;
	default:
		break;
	}


	std::vector<glm::dvec3> seq = buildSequence(initialInput, nTicker, 1000);
	//normaliseSet(seq, glm::vec3{ max });
	m_cubePositions.clear();
	m_cubeColours.clear();
	m_cubePositions = doubleToFloat(seq);
	m_cubeColours.resize(m_cubePositions.size());
	for (size_t i = 0; i < m_cubeColours.size(); ++i)
		m_cubeColours[i] = glm::vec3{ sin(i * 0.06 * glfwGetTime()),cos(i * 0.12 * glfwGetTime()),sin(i * 0.18 * glfwGetTime()) };
	

	//m_lineStripData = buildFractalData(c);
	//glBindBuffer(GL_ARRAY_BUFFER, m_lineStripVBO);
	//glBufferData(GL_ARRAY_BUFFER, sizeof(float) * seq.size(), &seq[0], GL_STATIC_DRAW);
	//glBindBuffer(GL_ARRAY_BUFFER, 0);

	/* SPAGHETTI IS GOOD WHEN EXPERIMENTING */
	//MetaDistribution<double> dist{ 0.0f,0.01f,0.1f,0.01f };

	//std::vector<std::vector<kiss_fft_cpx>> res = buf.fft();
	//std::vector<std::vector<kiss_fft_cpx>> reduced;
	//reduced.resize(res.size());
	//for (int i = 0; i < res.size(); ++i)
	//{
	//	// Take Middle
	//	int size = res[i].size();
	//	for (int j = (size / 4); j < (size * 3.0 / 4.0); ++j)
	//		reduced[i].push_back(res[i][j]);
	//}
	//double maxIVal = 0.0f;
	//AbstractRandomObject a;

	//std::normal_distribution<double> metaDistX{ dist.createDistribution() };
	//std::normal_distribution<double> metaDistY{ dist.createDistribution() };
	//std::normal_distribution<double> metaDistZ{ dist.createDistribution() };
	//double metaOffsetX{ metaDistX(a.mersenneTwister()) }, metaOffsetY{ metaDistY(a.mersenneTwister()) }, metaOffsetZ{ metaDistZ(a.mersenneTwister()) };


	//std::map<int, std::deque<float>> & data = buf.data();
	//std::deque<float> & ch = data.at(0);

	//for (int i = 0; i < 5; ++i)
	//{
	//	int posX = i;
	//	int posY = (reduced[0].size() / 2) + i;
	//	int posZ = (reduced[0].size() - 1) - i;
	//	double meanX = reduced[0][posX].r;
	//	double meanY = reduced[0][posY].r;
	//	double meanZ = reduced[0][posZ].r;
	//	double rotateX = (i % 3 == 0) ? meanX : ((i % 3 == 1) ? meanY : meanZ);
	//	double rotateY = (i % 3 == 0) ? meanY : ((i % 3 == 1) ? meanZ : meanX);
	//	double rotateZ = (i % 3 == 0) ? meanZ : ((i % 3 == 1) ? meanX : meanY);

	//	double normalisation = std::max(std::max(std::abs(meanX), std::abs(meanY)), std::abs(meanZ));

	//	double colX{ 0.0 }, colY{ 0.0 }, colZ{ 0.0 };
	//	for (unsigned i = 0u; i < ch.size(); ++i)
	//	{
	//		if (i % 3 == 0)
	//			if (colX < ch[i])
	//				colX = ch[i];
	//		if (i % 7 == 0)
	//			if (std::abs(colY) < std::abs(ch[i]))
	//				colY = abs(ch[i]);
	//		if (i % 2 == 0)
	//			if (colZ < ch[i])
	//				colZ = ch[i];
	//	}
	//	std::normal_distribution<double> xCol{ colX,0.1 };
	//	std::normal_distribution<double> yCol{ colY,0.2 };
	//	std::normal_distribution<double> zCol{ colZ,0.4 };

	//	glm::vec3 cubeColour{ std::abs(xCol(a.mersenneTwister())), std::abs(yCol(a.mersenneTwister())),  std::abs(zCol(a.mersenneTwister())) };

	//	std::normal_distribution<double> xPos{ rotateX, 0.02f + std::abs(metaOffsetX) };
	//	std::normal_distribution<double> yPos{ rotateY, 0.02f + std::abs(metaOffsetY) };
	//	std::normal_distribution<double> zPos{ rotateZ, 0.02f + std::abs(metaOffsetZ) };
	//	glm::vec3 cubePos{ xPos(a.mersenneTwister()),yPos(a.mersenneTwister()), zPos(a.mersenneTwister()) };

	//	m_cubePositions.insert(m_cubePositions.begin(), cubePos);
	//	m_cubeColours.insert(m_cubeColours.begin(), cubeColour);
	//}

	//double signalPowerVal{ 0.0 };
	//for (int i = 0; i < ch.size(); ++i)
	//	signalPowerVal += (ch[i] * ch[i]);
	//signalPowerVal /= (double)ch.size();

	//m_signalPowerHistory.push_back(signalPowerVal);
	//if (m_signalPowerHistory.size() > 10)
	//	m_signalPowerHistory.pop_front();

	//double signalPowerAverage{ 0.0 };
	//for (size_t i = 0; i < m_signalPowerHistory.size(); ++i)
	//	signalPowerAverage += m_signalPowerHistory[i];
	//signalPowerAverage /= (double)m_signalPowerHistory.size();


	///* TODO: Figure out a good relationship for the decay rate.. */
	//while (m_cubePositions.size() > 500)
	//{
	//	m_cubePositions.pop_back();
	//	m_cubeColours.pop_back();
	//}

	//if (m_cubeColours.size() != m_cubePositions.size())
	//	m_cubeColours.resize(m_cubePositions.size(), glm::vec3{ 1.0f,0.0f,0.0f });

	m_camera.m_position.x = 3.0f * sinf(glfwGetTime());
	m_camera.m_position.z = 3.0f * cosf(glfwGetTime());
	double yawRadians = atan2(m_camera.m_position.x, m_camera.m_position.z);
	double yawDegrees = glm::degrees(yawRadians);

	glm::vec3 front;
	front.x = -m_camera.m_position.x;
	front.y = -m_camera.m_position.y;
	front.z = -m_camera.m_position.z;
	m_camera.m_front = glm::normalize(front);
}

void Visualisation_Fractal::renderFrame()
{
	// per-frame time logic
	// --------------------
	float currentFrame = glfwGetTime();
	m_deltaTime = currentFrame - m_lastFrame;
	m_lastFrame = currentFrame;

	// activate shader
	m_objectShader->use();
	m_objectShader->setVec3("lightColour", glm::vec3{ 1.0f,0.5f,0.31f });
	m_objectShader->setVec3("objectColour", glm::vec3{ 1.0f,0.5f,0.31f });
	// pass projection matrix to shader (note that in this case it could change every frame)
	glm::mat4 projection = glm::perspective(glm::radians(m_camera.m_zoom), (float)1920 / (float)1080, 0.1f, 100.0f);
	m_objectShader->setMat4("projection", projection);

	// camera/view transformation
	glm::mat4 view = m_camera.GetViewMatrix();
	m_objectShader->setMat4("view", view);
	glm::mat4 lightModel{ 1.0f };
	m_lightPos = glm::vec3(10 * sin(glfwGetTime()), 10 * cos(glfwGetTime()), 10 * sin(glfwGetTime()));
	lightModel = glm::translate(lightModel, m_lightPos);
	lightModel = glm::scale(lightModel, glm::vec3{ 0.2f });
	m_objectShader->setVec3("lightPos", m_lightPos);
	m_objectShader->setVec3("viewPos", m_camera.m_position);

	// render boxes
	//m_lineStripShader->use();
	//glBindVertexArray(m_lineStripVAO);
	//glDrawArrays(GL_POINTS, 0, m_lineStripData.size());
	// render boxes
	glBindVertexArray(m_cubeVAO);
	for (unsigned int i = 0; i < m_cubePositions.size(); i++)
	{
		// calculate the model matrix for each object and pass it to shader before drawing
		glm::mat4 model{ 1.0f };
		model = glm::translate(model, m_cubePositions[i]);
		model = glm::scale(model, glm::vec3{ 0.02f,0.02f,0.02f });

		//*m_cubeColours[i].r * m_cubeColours[i].g * m_cubeColours[i].g
		float angle = 0.4f * (float)i;
		model = glm::rotate(model, glm::radians(angle), glm::vec3(1.0f, 0.3f, 0.5f));
		m_objectShader->setMat4("model", model);

		m_objectShader->setVec3("objectColour", m_cubeColours[i]);
		glDrawArrays(GL_TRIANGLES, 0, 36);
	}


	m_lampShader->use();
	m_lampShader->setMat4("projection", projection);
	m_lampShader->setMat4("view", view);
	m_lampShader->setMat4("model", lightModel);
	glBindVertexArray(m_lightVAO);
	glDrawArrays(GL_TRIANGLES, 0, 36);
}

// Taken from: https://rosettacode.org/wiki/Julia_set#C.2B.2B
int Visualisation_Fractal::inSet(std::complex<long double> z, std::complex<long double> c)
{
	for (int ec = 0; ec < m_iterations; ec++) {
		z = z * z;
		z = z + c;
		long double dist{ (z.imag() * z.imag()) + (z.real() * z.real()) };
		if (dist > 3.0l) return(ec);
	}
	return 0;
}

std::vector<glm::vec3> Visualisation_Fractal::buildFractalData(std::complex<long double> k)
{
	std::vector<glm::vec3> result;
	int res, pos;
	int BMP_SIZE = 100;
	std::complex<long double> c, factor(m_FCT / BMP_SIZE, m_FCT / BMP_SIZE);
	for (int y = 0; y < BMP_SIZE; y++)
	{
		pos = y * BMP_SIZE;
		c.imag((factor.imag() * y) + -m_hFCT);
		for (int x = 0; x < BMP_SIZE; x++)
		{
			glm::vec3 colourVector{ 0.0f,0.0f,0.0f };
			glm::vec3 positionVector{ (2.0 * (double)x / (double)BMP_SIZE) - 1.0, (2.0 * (double)y / (double)BMP_SIZE) - 1.0, 0.0f };

			c.real(factor.real() * x + -m_hFCT);
			res = inSet(c, k);
			if (res) {
				int n_res = res % 255;
				if (res < (m_iterations >> 1))
				{
					res = RGB(n_res << 2, n_res << 3, n_res << 4);
					colourVector.r = (double)(n_res << 2) / (double)255;
					colourVector.g = (double)(n_res << 3) / (double)255;
					colourVector.b = (double)(n_res << 4) / (double)255;
				}
				else
				{
					res = RGB(n_res << 4, n_res << 2, n_res << 5);
					colourVector.r = (double)(n_res << 4) / (double)255;
					colourVector.g = (double)(n_res << 2) / (double)255;
					colourVector.b = (double)(n_res << 5) / (double)255;
				}
			}
			/* Two parts: Position & Colour information. */
			result.push_back(positionVector);
			result.push_back(colourVector);
		}
	}
	return result;
}
