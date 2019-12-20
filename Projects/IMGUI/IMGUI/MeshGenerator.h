#pragma once
#include "Mesh.h"
#include "MetaDistribution.h"

namespace MeshGenerator
{
	void generateCube(Mesh & m)
	{
		std::vector<MeshVertex> vxs{
MeshVertex{	-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f},
MeshVertex{0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f},
MeshVertex{0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f},
MeshVertex{0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f},
MeshVertex{-0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f},
MeshVertex{-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f},

MeshVertex{-0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f},
MeshVertex{0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f},
MeshVertex{0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f},
MeshVertex{0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f},
MeshVertex{-0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f},
MeshVertex{-0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f},

MeshVertex{-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f},
MeshVertex{-0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f},
MeshVertex{-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f},
MeshVertex{-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f},
MeshVertex{-0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f},
MeshVertex{-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f},

MeshVertex{0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f},
MeshVertex{0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f},
MeshVertex{0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f},
MeshVertex{0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f},
MeshVertex{0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f},
MeshVertex{0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f},

MeshVertex{-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f},
MeshVertex{0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f},
MeshVertex{0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f},
MeshVertex{0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f},
MeshVertex{-0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f},
MeshVertex{-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f},

MeshVertex{-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f},
MeshVertex{0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f},
MeshVertex{0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f},
MeshVertex{0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f},
MeshVertex{-0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f},
		MeshVertex {-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f} };

		std::vector<unsigned int> idxs
		{
			0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31,32,33,34,35
		};
		m.regenerateMesh(vxs, idxs);
	}
	void generateSquare(Mesh & m)
	{
		std::vector<MeshVertex> vxs = {
		MeshVertex{ 0.5f, 0.5f, 0.0f },
			MeshVertex{ 0.5f, -0.5f, 0.0f },  // bottom right
			MeshVertex{ -0.5f, -0.5f, 0.0f },  // bottom left
			MeshVertex{ -0.5f, 0.5f, 0.0f }  // top left 
		};
		std::vector<unsigned int> idxs =
		{
			0, 1, 3,   // first triangle
			1, 2, 3    // second triangle
		};
		m.regenerateMesh(vxs, idxs);
	}

	// z = 0 for now.
	void generateGraph(unsigned int width, unsigned int height, Mesh& m)
	{
		// always -1 to 1
		assert(width > 1 && height > 1);

		// temp for z
		std::normal_distribution<double> xCol{ 0.0,0.02 };
		AbstractRandomObject a;

		
		double wInterval{ 1 / (double)width };
		double hInterval{1 / (double)height};
		unsigned int counter{ 0 };

		std::vector<MeshVertex> vxs{};
		/* Vertex Positions */
		for (unsigned int w = 0; w <= width; ++w)
		{
			double wPos = ((double)w * wInterval) - 0.5;
			for (unsigned int h = 0; h <= height; ++h)
			{
				double hPos = ((double)h * hInterval) - 0.5;
				vxs.push_back(glm::vec3{wPos,hPos, xCol(a.mersenneTwister())});
				++counter;
			}
		}
		/* Triangle indexes */
		std::vector<unsigned int> idxs{};
		for (unsigned int w = 0; w < width; ++w)
		{
			for (unsigned int h = 0; h < height; ++h)
			{
				std::vector<unsigned int> j{((height+1)*w)+h, ((height+1)*w) +h + 1, ((height+1)*w) +h+height+1,  ((height+1) * w) +h+1, ((height + 1) * w) +h+height+2, ((height + 1) * w) +h+height+1};
				idxs.insert(idxs.end(),j.begin(),j.end());
			}
		}
		/* Smooth normals */
		for (int i = 0; i < idxs.size(); i += 3)
		{
			glm::vec3 p = glm::cross(vxs[idxs[i+1]].Position - vxs[idxs[i]].Position, vxs[idxs[i+2]].Position - vxs[idxs[i]].Position);
			vxs[idxs[i]].Normal += p;
			vxs[idxs[i+1]].Normal += p;
			vxs[idxs[i+2]].Normal += p;
		}
		for (int i = 0; i < vxs.size(); ++i)
			vxs[i].Normal = glm::normalize(vxs[i].Normal);

		m.regenerateMesh(vxs, idxs);
	}

}