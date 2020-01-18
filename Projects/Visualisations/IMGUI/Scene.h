#pragma once
#include "Mesh.h"
#include <map>

/* A scene will likely be useful for the tree-field visualisation. */
class Scene
{
public:

	void draw()
	{
		for (auto i = m_models.begin(); i != m_models.end(); ++i)
		{
			const unsigned int& k = i->first;
			auto m = m_meshes.find(k);
			auto s = m_shaders.find(k);
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
	std::map<unsigned int, glm::mat4> m_models;

	/* Shaders (wait do you need a multimap here?)*/
	std::multimap<unsigned int, std::shared_ptr<Shader>> m_shaders;
	std::multimap<unsigned int, Mesh> m_meshes;
};