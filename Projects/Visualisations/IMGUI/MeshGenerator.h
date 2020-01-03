#pragma once
#include "Mesh.h"
#include "MetaDistribution.h"
#include <deque>

namespace MeshGenerator
{
	void generateCube(Mesh& m);
	void generateSquare(Mesh& m);
	// z = 0 for now.
	void generateGraph(unsigned int width, unsigned int height, Mesh& m, std::deque<float> zData = std::deque<float>{});
	void generateSphere(unsigned int res, Mesh& m);
	void generateCylinder(unsigned int res, float height, float radius, Mesh& m);
	// psi takes zero to pi/2
	void generateCone(unsigned int res, float height, float radius, Mesh& m);
	void generateTorus(unsigned int res, float innerRadius, float outerRadius, Mesh& m);
	void generateArrow(unsigned int res, Mesh& m);
 }
