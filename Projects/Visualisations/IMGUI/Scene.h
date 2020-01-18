#pragma once
#include "Mesh.h"
#include <map>


typedef unsigned int shaderID;
typedef unsigned int meshID;
typedef unsigned int modelID;

/* A scene will likely be useful for the tree-field visualisation. */
class Scene
{
public:
	Scene()
		:
		m_lastModel{ 0 },
		m_lastMesh{0},
		m_lastShader{0},

		m_meshes{},
		m_meshIDs{},
		m_models{},
		m_shaderIDs{},
		m_shaders{}
	{

	}

	void addModel(const glm::mat4& model, std::shared_ptr<Mesh>& mesh, Shader* shader)
	{
		/*
			1. a. get new model id
				b. add model. 
			2. a. see if shader already created
				if so:
					get shader id
				else:
					add shader, with new last shader id
			3. Do the same for mesh.
		*/
		modelID newModelID = ++m_lastModel;
		//auto existingShader = m_shaders.find(shader);
		//auto existingModel = m_models.find();

	}

	void draw()
	{
		for (auto i = m_models.begin(); i != m_models.end(); ++i)
		{
			const modelID& moID = i->first;
			const glm::mat4& model = i->second;

			auto meID = m_meshIDs.find(moID);
			auto shID = m_shaderIDs.find(moID);

			auto mesh = m_meshes.find(meID->second);
			auto shader = m_shaders.find(shID->second);
			if (mesh != m_meshes.end() && shader != m_shaders.end())
			{
				shader->second->setMat4("model", model);
				mesh->second->draw(shader->second);
			}
			//m->second.draw(s->second);
		}
	}
private:
	std::vector<unsigned int> keys()
	{
		std::vector<unsigned int> result{};
		// TODO: how do you quickly get keys from a multimap?
		return result;
	}
	/* An index-modelmatrix key-value pair is unique per model.*/
	std::map<modelID, glm::mat4> m_models;
	std::map<modelID, shaderID> m_shaderIDs;
	std::map<modelID, meshID> m_meshIDs;

	/* Shaders, TODO: shared_ptr<Shader> */
	std::map<shaderID, Shader*> m_shaders;
	std::map<meshID, std::shared_ptr<Mesh>> m_meshes;

	modelID m_lastModel;
	shaderID m_lastShader;
	meshID m_lastMesh;
};