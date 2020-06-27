#pragma once
#include "Mesh.h"
#include "MetaDistribution.h"
#include <deque>

namespace MeshGenerator
{
	void generateUnknownSurface(Mesh& m, float a = 1.0, float b = 1.5, float c = 2.4);


	void generateCube(Mesh& m);
	void generateSquare(Mesh& m);
	void generateGraph(unsigned int width, unsigned int height, Mesh& m, std::deque<float> zData = std::deque<float>{});
	void generateSphere(unsigned int res, Mesh& m);
	void generateCylinder(unsigned int res, float height, float radius, Mesh& m);

	void generateCylinder(Mesh& m, glm::vec3& start, glm::vec3& end, unsigned int res, float startRadius = 1.0f, float endRadius = 1.0f);

	void generateCone(unsigned int res, float height, float radius, Mesh& m);
	void generateTorus(unsigned int res, float innerRadius, float outerRadius, Mesh& m);
	void generateArrow(unsigned int res, Mesh& m);




	/* Connector for branches in tree */
	void generateConnector(unsigned int res, float r1, float r2, float d1, float d2, Mesh& m);


	enum MandelbulbVersion
	{
		Wikipedia,	// https://en.wikipedia.org/wiki/Mandelbulb
		Bugman,		// http://bugman123.com/Hypercomplex/
		Twinbee,	// http://www.fractalforums.com/3d-fractal-generation/true-3d-mandlebrot-type-fractal/msg4109/#msg4109
	};
	std::vector<glm::vec3> generateMandelbulb(unsigned int res, double n, glm::vec3 initialPos);
 }
