/*
Features:
	1. Implement mandelbrot/julia sets as surfaces
	2. Redraw sets based on camera distance.
	3. Programatic texture loading and generation:
		-> Mesh-based specular map generation
	4. Skybox
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
		VAO{ 0 }
	{
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
private:
	void gfxInit();
	void gfxDelete();

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
