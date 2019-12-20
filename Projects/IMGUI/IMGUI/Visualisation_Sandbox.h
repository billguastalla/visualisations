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

	Shader* m_objectShader;
	Shader* m_lampShader;

	std::deque<double> m_signalPowerHistory;

	glm::vec3 m_lightPos;
	
	Mesh m_meshTop, m_meshBottom;
	Mesh m_lightMesh;
};