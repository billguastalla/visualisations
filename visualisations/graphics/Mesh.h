/*
Features:
	1. Implement mandelbrot/julia sets as surfaces
	2. Redraw sets based on camera distance.
	3. Programatic texture loading and generation:
		-> Mesh-based specular map generation
	4. Skybox
	5. Other surfaces:
		-> Mobius strip,
		->
*/
#pragma once
#include <glad/glad.h>


#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "shader.h"
#include "Texture.h"

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>

using std::vector;

struct MeshVertex
{
	MeshVertex()
		:
		Position{},
		Normal{},
		TexCoords{},
		Tangent{},
		Bitangent{}
	{};
	MeshVertex(glm::vec3 pos, glm::vec3 norm = glm::vec3{ 0.0,0.0,1.0 }, glm::vec2 tex = glm::vec2{ 0.0 }, glm::vec3 tan = glm::vec3{ 0.0 }, glm::vec3 bitan = glm::vec3{ 0.0 })
		: Position{ pos }, Normal{ norm }, TexCoords{ tex }, Tangent{ tan }, Bitangent{ bitan }
	{}
	MeshVertex(float p1, float p2, float p3, float norm1 = 0.0f, float norm2 = 0.0f, float norm3 = 1.0f)
		: Position{ p1,p2,p3 }, Normal{ norm1,norm2,norm3 }, TexCoords{ 0.0f }, Tangent{ 0.0f }, Bitangent{ 0.0f }
	{}
	// position
	glm::vec3 Position;
	// normal
	glm::vec3 Normal;
	// texCoords
	glm::vec2 TexCoords;
	// tangent
	glm::vec3 Tangent;
	// bitangent
	glm::vec3 Bitangent;
};

class Mesh {
public:
	Mesh(vector<MeshVertex> v = vector<MeshVertex>{}, vector<unsigned int> i = vector<unsigned int>{}, vector<Texture> t = vector<Texture>{})
		:
		m_vertices{ v },
		m_indices{ i },
		m_textures{ t },
		m_gfxInitialised{ false },
		EBO{ 0 },
		VBO{ 0 },
		VAO{ 0 },
		m_normalArrows{ nullptr }
	{
	}
	/* Copy */
	Mesh(const Mesh& other)
		:
		m_vertices{ other.m_vertices },
		m_indices{ other.m_indices },
		m_textures{ other.m_textures },
		m_gfxInitialised{ false },
		EBO{ 0 },
		VBO{ 0 },
		VAO{ 0 },
		m_normalArrows{ nullptr }
	{
	}
	Mesh& operator=(const Mesh & other)
	{
		m_vertices = other.m_vertices;
		m_indices = other.m_indices;
		m_textures = other.m_textures;
		m_gfxInitialised = false;
		EBO = 0;
		VBO = 0;
		VAO = 0;
		m_normalArrows = nullptr;
		return *this;
	}


	void regenerateMesh(vector<MeshVertex> v, vector<unsigned int> i, vector<Texture> t = vector<Texture>{})
	{
		gfxDelete();
		m_vertices = v;
		m_indices = i;
		m_textures = t;
	}
	void addTexture(const Texture& t);

	void draw(Shader* shader);

	void appendMesh(const Mesh& other);
	void translate(const glm::vec3& pos);
	void scale(const glm::vec3& mag);
	void rotate(const glm::vec3& axis, float angle);

	void showNormals(bool y);


	bool interpolate(const Mesh& m1, const Mesh& m2, float extent)
	{
		// assert(m1.m_vertices.size() == m2.m_vertices.si) {...}
		if (m1.m_vertices.size() != m2.m_vertices.size())
			return false;

		m_indices = m1.m_indices;
		m_textures = m1.m_textures;
		m_vertices.resize(m1.m_vertices.size());

		std::vector<MeshVertex>::const_iterator m1Vertex = m1.m_vertices.begin();
		std::vector<MeshVertex>::const_iterator m2Vertex = m2.m_vertices.begin();
		std::vector<MeshVertex>::iterator vertex = m_vertices.begin();
		while (m1Vertex != m1.m_vertices.end() && m2Vertex != m2.m_vertices.end())
		{
			vertex->Normal = ((extent * m2Vertex->Normal) + ((1 - extent) * m1Vertex->Normal));
			vertex->Bitangent = ((extent * m2Vertex->Bitangent) + ((1 - extent) * m1Vertex->Bitangent));
			vertex->Position = ((extent * m2Vertex->Position) + ((1 - extent) * m1Vertex->Position));
			vertex->Tangent = ((extent * m2Vertex->Tangent) + ((1 - extent) * m1Vertex->Tangent));
			vertex->TexCoords = ((extent * m2Vertex->TexCoords) + ((1 - extent) * m1Vertex->TexCoords));
			++m1Vertex;
			++m2Vertex;
			++vertex;
		}
		/* todo: only update mesh data */
		gfxDelete();
		return true;
	}

	/* These are safe to be public */
	void gfxInit();
	void gfxDelete();

	void clear() {
		gfxDelete();
		m_vertices.clear();
		m_indices.clear();
		m_textures.clear();
	}
private:
	/* Normals */
	std::unique_ptr<Mesh> m_normalArrows;


	/*  CPU Data  */
	vector<MeshVertex> m_vertices;
	vector<unsigned int> m_indices;
	vector<Texture> m_textures;

	/* Initialised */
	bool m_gfxInitialised;

	/* GFX Handles */
	unsigned int VAO;
	unsigned int VBO, EBO;
};
