#include "Mesh.h"
#include "MeshGenerator.h"

#include <GLM/gtc/quaternion.hpp>
using std::string;

void Mesh::addTexture(const Texture& t)
{
	m_textures.push_back(t);
}

void Mesh::draw(Shader* shader)
{
	gfxInit();

	// bind appropriate m_textures
	unsigned int diffuseNr = 1;
	unsigned int specularNr = 1;
	unsigned int normalNr = 1;
	unsigned int heightNr = 1;
	for (unsigned int i = 0; i < m_textures.size(); i++)
	{
		glActiveTexture(GL_TEXTURE0 + i); // active proper texture unit before binding
		// retrieve texture number (the N in diffuse_textureN)
		string number;

		string name = Texture::typeStr(m_textures[i].t);
		switch (m_textures[i].t)
		{
		case Texture::Type::Diffuse:
			number = std::to_string(diffuseNr++); break;
		case Texture::Type::Specular:
			number = std::to_string(specularNr++); break; // transfer unsigned int to stream 
		case Texture::Type::Normal:
			number = std::to_string(normalNr++); break; // transfer unsigned int to stream
		case Texture::Type::Height:
			number = std::to_string(heightNr++); break; // transfer unsigned int to stream
		default: break;
		}
		// now set the sampler to the correct texture unit
		glUniform1i(glGetUniformLocation(shader->ID, (name + number).c_str()), i);
		// and finally bind the texture
		glBindTexture(GL_TEXTURE_2D, m_textures[i].id);
	}

	// draw mesh
	glBindVertexArray(VAO);
	glDrawElements(GL_TRIANGLES, m_indices.size(), GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);

	// always good practice to set everything back to defaults once configured.
	glActiveTexture(GL_TEXTURE0);

	if (m_normalArrows.get() != nullptr)
		m_normalArrows->draw(shader);
}

void Mesh::appendMesh(const Mesh& other)
{
	std::vector<unsigned int> indices{ other.m_indices };
	for (auto i = indices.begin(); i != indices.end(); ++i)
		* i += m_vertices.size();
	m_indices.insert(m_indices.end(), indices.begin(), indices.end());
	m_vertices.insert(m_vertices.end(), other.m_vertices.begin(), other.m_vertices.end());
	m_textures.insert(m_textures.end(), other.m_textures.begin(), other.m_textures.end());

	//give me a cone =cone
	//one shall keep this. okay.
}

void Mesh::translate(const glm::vec3& pos)
{
	for (auto i = m_vertices.begin(); i != m_vertices.end(); ++i)
		i->Position += pos;
}

void Mesh::scale(const glm::vec3& mag)
{
	for (auto i = m_vertices.begin(); i != m_vertices.end(); ++i)
		i->Position *= mag;
}

void Mesh::rotate(const glm::vec3& axis, float angle)
{
	/* Two types:
		1. Rotation by quaternions
		2. Rotation by Euler angles
		Two features:
			-> Rotation of normal arrows,
			-> Rotation of anything.
	*/
	// todo: also you need more information in params for anything with less than two-fold symmetry.

	// todo: Split this geometry away from meshes, and leave this stuff to the model matrices.
	glm::quat rot{};
	rot.x = axis.x * sin(angle / 2);
	rot.y = axis.y * sin(angle / 2);
	rot.z = axis.z * sin(angle / 2);
	rot.w = cos(angle / 2);
	for (auto i = m_vertices.begin(); i != m_vertices.end(); ++i)
	{
		i->Position = rot * i->Position;
		//i->Normal = rot * i->Normal; // check
	}
}

void Mesh::showNormals(bool y)
{
	if (y && (m_normalArrows.get() == nullptr))
	{
		Mesh m{};
		MeshGenerator::generateArrow(5, m);
		m.scale(glm::vec3{ 0.001,0.001,0.001 });
		m_normalArrows.reset(new Mesh{});

		for (auto i = m_vertices.begin(); i != m_vertices.end(); ++i)
		{
			Mesh v{};
			v.appendMesh(m);
			v.translate(i->Position);
			
			/* TODO: Think about how to do three rotations on an arrow pointing upwards in the z-direction*/
			//v.rotate(i->Normal);
			glm::vec3& n = i->Normal;
			float angleX = asin(n.y / n.x);
			float angleY = asin(n.y / n.x);
			glm::vec3 axisX{ 1.0,0.0,0.0 };
			glm::vec3 axisY{ 0.0,1.0,0.0 };
			v.rotate(axisY,angleX);
			v.rotate(axisX,angleY);
			m_normalArrows->appendMesh(v);
		}
	}
	else if (!y && (m_normalArrows.get() != nullptr))
		m_normalArrows.reset(nullptr);
}


void Mesh::gfxInit()
{
	if (!m_gfxInitialised)
	{
		// create buffers/arrays
		glGenVertexArrays(1, &VAO);
		glGenBuffers(1, &VBO);
		glGenBuffers(1, &EBO);

		glBindVertexArray(VAO);
		// load data into vertex buffers
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		// A great thing about structs is that their memory layout is sequential for all its items.
		// The effect is that we can simply pass a pointer to the struct and it translates perfectly to a glm::vec3/2 array which
		// again translates to 3/2 floats which translates to a byte array.
		glBufferData(GL_ARRAY_BUFFER, m_vertices.size() * sizeof(MeshVertex), &m_vertices[0], GL_STATIC_DRAW);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_indices.size() * sizeof(unsigned int), &m_indices[0], GL_STATIC_DRAW);

		// set the vertex attribute pointers
		// vertex Positions
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(MeshVertex), (void*)0);
		// vertex normals
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(MeshVertex), (void*)offsetof(MeshVertex, Normal));
		// vertex texture coords
		glEnableVertexAttribArray(2);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(MeshVertex), (void*)offsetof(MeshVertex, TexCoords));
		// vertex tangent
		glEnableVertexAttribArray(3);
		glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(MeshVertex), (void*)offsetof(MeshVertex, Tangent));
		// vertex bitangent
		glEnableVertexAttribArray(4);
		glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(MeshVertex), (void*)offsetof(MeshVertex, Bitangent));

		glBindVertexArray(0);
		m_gfxInitialised = true;
	}
}

void Mesh::gfxDelete()
{
	if (m_gfxInitialised)
	{
		glDeleteVertexArrays(1, &VAO);
		glDeleteBuffers(1, &VBO);
		glDeleteBuffers(1, &EBO);
		VAO = 0;
		VBO = 0;
		EBO = 0;
		m_gfxInitialised = false;
	}
}
