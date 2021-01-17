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
		m_lastMesh{ 0 },
		m_lastShader{ 0 },

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

/*
	-> Like with meshes, interpolating a scene should move vertices gradually
		across to the vertices of the new scene.
	-> Given that meshes in a scene might move from other motion as interpolation happens,
		a choice needs to be made at the beginning of the interpolation,
			to find which meshes to move to which.
	-> Mesh count can be different, and the vertex count can be different too.
	-> Before this is written, a mesh should be able to merge itself and split
		itself into any number of meshes.
	-> Need to be able to find the nearest model for a vertex, and nearest models for a mesh.
	-> This requires understanding how the model matrix is applied to vertices,
		and also an ability to break the indices up so that parts of a mesh can be
			extracted into a new mesh.
*/
class InterpolatedScene : public Scene
{
	InterpolatedScene(std::shared_ptr<Scene> scene1, std::shared_ptr<Scene> scene2)
		: Scene{},
		m_scene1{ scene1 },
		m_scene2{ scene2 },
		m_extent{0.0f}
	{}

	void initialise() {};
	bool interpolate(float extent) {};


	std::shared_ptr<Scene> m_scene1;
	std::shared_ptr<Scene> m_scene2;
	float m_extent;
};