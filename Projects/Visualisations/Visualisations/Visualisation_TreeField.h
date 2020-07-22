#pragma once
#include "Visualisation.h"
#include "Mesh.h"
#include "MeshGenerator.h"

#include "VectorField.h"

class Buffer;
struct GLFWwindow;
class Shader;

class Visualisation_TreeField : public Visualisation
{
public:
	Visualisation_TreeField();
	void activate() override;
	void deactivate() override;
	void processSamples(const Buffer & buf, unsigned samples) override;
	void renderFrame(const Camera& camera, const Timestep& t) override;
	std::string titleString() override { return "Tree/Field"; };
	void drawInterface() override;

private:
	Shader * m_objectShader;
	Shader * m_lampShader;
	glm::vec3 m_lightPos;

	Mesh m_lamp;

	VectorField m_field;
};