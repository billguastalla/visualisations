#pragma once
#include "Mesh.h"
#include "MeshGenerator.h"

/* 
	f: R^3 -> R^3
	f(x,y,z) = ..

	Problem is glm doesnt have function to rotate a matrix by a vector:
						-> This is because matrices don't have the spherical symmetry of arrows.
						-> need to calculate, 
*/
class VectorField
{
public:
	VectorField(int arrowRes = 5, float cellSize = 5.0f, float drawDistance = 100.0f, float arrowSize = 0.2f)
		:
		m_arrow{},
		m_arrowShader{ new Shader{"../Shaders/Cubes_Vertex.vs","../Shaders/Cubes_ObjectFragment.fs"} },
		m_cellSize{ cellSize },
		m_drawDistance{ drawDistance },
		m_arrowSize{arrowSize},

		m_orientations{}
	{

		MeshGenerator::generateArrow(arrowRes, m_arrow);
		m_arrow.scale(glm::vec3{arrowSize});
	};
	/* Draw arrows up to the draw distance of the camera position. */
	void draw(glm::vec3 cameraPos)
	{
		//adjust(cameraPos);
		for (auto o : m_orientations)
		{
			m_arrowShader->setMat4("model", glm::translate(o.mod,o.pos));
			m_arrow.draw(m_arrowShader.get());
		}
	};

	void adjust(const glm::vec3& cameraPos)
	{
		/* Don't do this, optimise away asap. */
		setup();
	}
	void setup()
	{
		m_orientations.clear();
		float count{ (float)std::floor(m_drawDistance / m_cellSize) };
		for (float x{ 0.0f }; x < m_drawDistance; x += m_cellSize)
			for (float y{ 0.0f }; y < m_drawDistance; y += m_cellSize)
				for (float z{ 0.0f }; z < m_drawDistance; z += m_cellSize)
				{

					glm::mat4 m{ 1.0f };
					m_orientations.push_back(
						orientation{
							glm::vec3{ x,y,z },
							m
						}
					);
				}
	}
private:
	struct orientation {
		orientation(glm::vec3& p, glm::mat4 m) : pos{ p }, mod{ m } {}
		glm::vec3 pos;
		glm::mat4 mod;
	};
	/* Positions generated first, from scale, camerapos and draw distance */
	std::vector<orientation> m_orientations;
	std::unique_ptr<Shader> m_arrowShader;
	Mesh m_arrow;

	float m_cellSize;
	float m_arrowSize;
	float m_drawDistance;
};
