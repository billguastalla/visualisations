#pragma once
#include "Mesh.h"
#include "MeshGenerator.h"

class VectorField
{
public:
	VectorField(int arrowRes = 20)
		:
		m_arrow{}
	{
		MeshGenerator::generateArrow(arrowRes, m_arrow);
	};
	/* Draw arrows up to the draw distance of the camera position. */
	void draw(glm::vec3 cameraPos)
	{
		m_arrow.draw(m_arrowShader.get());
	};
private:
	std::unique_ptr<Shader> m_arrowShader;
	Mesh m_arrow;
};
