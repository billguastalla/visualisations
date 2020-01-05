#include "MeshGenerator.h"

auto buildMeshIndices = [](const unsigned int & width, const unsigned int & height)
{
	std::vector<unsigned int> idxs;
	for (unsigned int w = 0; w < width; ++w)
		for (unsigned int h = 0; h < height; ++h)
		{
			std::vector<unsigned int> j{
				((height + 1) * w) + h,
				((height + 1) * w) + h + 1,
				((height + 1) * w) + h + width + 1,
				((height + 1) * w) + h + 1,
				((height + 1) * w) + h + width + 2,
				((height + 1) * w) + h + width + 1
			};
			idxs.insert(idxs.end(), j.begin(), j.end());
		}
	return idxs;
};
// WARNING: Set normals to zero before using. TODO: think about normals for primitives analytically, before jumping to this.
auto buildSmoothNormals = [](std::vector<MeshVertex> & vxs, const std::vector<unsigned int>& idxs)
{
	for (int i = 0; i < idxs.size(); i += 3)
	{
		glm::vec3 p = glm::cross(
			vxs[idxs[(size_t)i + 1]].Position - vxs[idxs[i]].Position,
			vxs[idxs[(size_t)i + 2]].Position - vxs[idxs[i]].Position);
		vxs[idxs[(size_t)i]].Normal += p;
		vxs[idxs[(size_t)i + 1]].Normal += p;
		vxs[idxs[(size_t)i + 2]].Normal += p;
	}
	for (int i = 0; i < vxs.size(); ++i)
		vxs[i].Normal = glm::normalize(vxs[i].Normal);
};


void MeshGenerator::generateCube(Mesh& m)
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
void MeshGenerator::generateSquare(Mesh& m)
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
void MeshGenerator::generateGraph(unsigned int width, unsigned int height, Mesh& m, std::deque<float> zData)
{
	assert(width > 1 && height > 1);
	std::normal_distribution<double> xCol{ 0.0,0.02 };
	AbstractRandomObject a;
	if (zData.empty())
		for (int i = 0; i < (width + 1) * (height + 1); ++i)
			zData.push_back(xCol(a.mersenneTwister()));
	double wInterval{ 1 / (double)width };
	double hInterval{ 1 / (double)height };
	unsigned int counter{ 0 };
	std::vector<MeshVertex> vxs{};
	/* Vertex Positions */
	for (unsigned int w = 0; w <= width; ++w)
	{
		double wPos = ((double)w * wInterval);
		for (unsigned int h = 0; h <= height; ++h)
		{
			double hPos = ((double)h * hInterval);
			vxs.push_back(glm::vec3{ wPos - 0.5,hPos - 0.5,zData[counter] });
			vxs[counter].TexCoords = glm::vec2{ wPos,hPos };
			++counter;
		}
	}
	/* Triangle indexes */
	std::vector<unsigned int> idxs{buildMeshIndices(width,height)};
	/* Smooth normals */
	buildSmoothNormals(vxs, idxs);

	m.regenerateMesh(vxs, idxs);
}
void MeshGenerator::generateSphere(unsigned int res, Mesh& m)
{
	assert(res >= 3);
	float thetaInterval = 2 * 3.14159265 / ((float)res);
	float phiInterval = 3.14159265 / ((float)res);
	std::vector<MeshVertex> vxs{};
	for (int i = 0; i <= res; ++i)
	{
		for (int j = 0; j <= res; ++j)
		{
			float theta{ (float)j * thetaInterval };
			float phi{ (float)i * phiInterval };
			glm::vec3 coord{ sin(phi) * cos(theta),sin(phi) * sin(theta),cos(phi) };
			glm::vec2 texCoord{ cos(theta),sin(phi) };
			vxs.push_back(MeshVertex{ coord,coord,texCoord });
		}
	}
	std::vector<unsigned int> idxs{ buildMeshIndices(res, res) };
	m.regenerateMesh(vxs, idxs);
}
void MeshGenerator::generateCylinder(unsigned int res, float height, float radius, Mesh& m)
{
	assert(res >= 3);
	float thetaInterval = 2 * 3.14159265 / ((float)res);
	float heightInterval = height / ((float)res);
	std::vector<MeshVertex> vxs{};
	for (int i = 0; i <= res; ++i)
	{
		for (int j = 0; j <= res; ++j)
		{
			float theta{ (float)j * thetaInterval };
			float h{ (float)i * heightInterval };
			glm::vec3 coord{ radius * cos(theta),radius * sin(theta),h };
			glm::vec3 norm{ radius * cos(theta),radius * sin(theta),radius / height };
			glm::vec2 texCoord{ cos(theta / 2.0), h };
			vxs.push_back(MeshVertex{ coord,norm,texCoord });
		}
	}
	std::vector<unsigned int> idxs{ buildMeshIndices(res, res) };
	m.regenerateMesh(vxs, idxs);
}
void MeshGenerator::generateCone(unsigned int res, float height, float radius, Mesh& m)
{
	assert(res >= 3);
	float thetaInterval = 2 * 3.14159265 / ((float)res);
	float radiusInterval = radius / ((float)res);
	float heightInterval = height / ((float)res);
	std::vector<MeshVertex> vxs{};
	for (int i = 0; i <= res; ++i)
	{
		for (int j = 0; j <= res; ++j)
		{
			float theta{ (float)j * thetaInterval };
			float r{ (float)(res - i) * radiusInterval };
			float h{ (float)i * heightInterval };
			glm::vec3 coord{ r * cos(theta),r * sin(theta),h };
			glm::vec2 texCoord{ cos(theta),h / height };
			vxs.push_back(MeshVertex{ coord,coord,texCoord });
		}
	}
	std::vector<unsigned int> idxs{ buildMeshIndices(res, res) };
	m.regenerateMesh(vxs, idxs);
}
void MeshGenerator::generateTorus(unsigned int res, float innerRadius, float outerRadius, Mesh& m)
{
	assert(res >= 3);
	float thetaInterval = 2 * 3.14159265 / ((float)res);
	float texInterval = 1.0 / ((float)res);
	std::vector<MeshVertex> vxs{};
	for (int i = 0; i <= res; ++i)
	{
		for (int j = 0; j <= res; ++j)
		{
			float thetaInner{ (float)j * thetaInterval };
			float thetaOuter{ (float)i * thetaInterval };
			glm::vec3 coord{
				(outerRadius + innerRadius * cos(thetaOuter)) * cos(thetaInner),
				(outerRadius + innerRadius * cos(thetaOuter)) * sin(thetaInner),
				innerRadius * sin(thetaOuter),
			};
			glm::vec2 texCoord{ texInterval * (float)i, texInterval * (float)j };
			vxs.push_back(MeshVertex{ coord,coord,texCoord });
		}
	}
	std::vector<unsigned int> idxs{ buildMeshIndices(res, res) };
	m.regenerateMesh(vxs, idxs);
}
void MeshGenerator::generateArrow(unsigned int res, Mesh& m)
{
	Mesh mCone{};
	generateCone(res, 2.0, 0.6, mCone);
	mCone.translate(glm::vec3{ 0.0,0.0,5.0 });
	generateCylinder(res, 5.0, 0.2, m);
	m.appendMesh(mCone);
}
