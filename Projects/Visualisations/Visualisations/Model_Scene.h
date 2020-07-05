#pragma once
#include "Shader.h"
#include "Texture.h"
#include <vector>
#include <map>
#include <boost/property_tree/ptree_fwd.hpp>

/* 
	Model_Scene will eventually replace Model_Visualisation, as each visualisation is turned into 
	a component.
		-> But: Visualisation hierarchy is probably still useful as a sandboxing system for the medium term.
*/
class Model_Scene
{

public:
	bool loadFileTree(const boost::property_tree::ptree& t);
	bool saveFileTree(boost::property_tree::ptree& t) const;

	std::map<int, Texture> m_textures;
	std::map<int, Shader> m_shaders;
};