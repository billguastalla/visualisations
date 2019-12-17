#pragma once
#include "Visualisation.h"
#include "Camera.h"
#include "Mesh.h"
#include <deque>
class Buffer;
struct GLFWwindow;
class Shader;

class Visualisation_Sandbox : public Visualisation
{
public:
	Visualisation_Sandbox();

	/* Set up callbacks, shaders, vertex objects */
	void activate() override;
	/* Free callbacks, shaders, vertex objects */
	void deactivate() override;

	void processSamples(const Buffer & buf, unsigned samples) override;
	void renderFrame() override;

	std::string titleString() override { return "Sandbox"; };
private:

	Shader * m_objectShader;

	std::deque<double> m_signalPowerHistory;

	glm::vec3 m_lightPos;
	glm::vec3 m_scaleVec;
	Mesh m_mesh;
};