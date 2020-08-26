#pragma once
#include "Shader.h"
#include "Texture.h"
#include <vector>
#include <map>
#include <boost/property_tree/ptree_fwd.hpp>
#include <boost/serialization/access.hpp>
/* 
	Model_Scene will eventually replace Model_Visualisation, as each visualisation is turned into 
	a component.
		-> But: Visualisation hierarchy is probably still useful as a sandboxing system for the medium term.
*/
class Model_Scene
{

public:

	std::map<int, Texture> m_textures;
	std::map<int, Shader> m_shaders;
	// Resource system (textures/shaders)
	// Mesh instances..
	// Skybox
	// ParticleSystems / settings

	
public:
	friend class boost::serialization::access;
	template<class Archive>
	void serialize(Archive& ar, const unsigned int version)
	{
		// Textures & shaders.. 
	}
};


/*
	Are events their own system?
*/	
