#pragma once
#include "Visualisation.h"
#include "Mesh.h"

#include <queue>

struct SplinterNode
{
	SplinterNode(glm::vec3 pos, SplinterNode * parent = nullptr, float scale = 1.0f)
		:
		m_position{pos},
		m_parent{parent},
		m_scale{scale}
	{
		
	}

	~SplinterNode()
	{
		for (SplinterNode* child : m_children)
			delete child;
		m_children.clear();
	}

	SplinterNode* m_parent;
	std::vector<SplinterNode*> m_children;
	glm::vec3 m_position;
	float m_scale;


	// Oscillation Properties
	glm::vec3 m_centrePos;
	glm::vec3 m_velocity;



	SplinterNode * addChild(glm::vec3 pos,float scale)
	{
		SplinterNode* child = new SplinterNode{pos,this,scale};
		m_children.push_back(child);
		return child;
	}

	void makeBranches(std::vector<unsigned> & branches, int depth = -1);
	void initialiseGeometry();
	std::vector<Mesh> build(Mesh m = Mesh{});

};

class Visualisation_Splinters : public Visualisation
{
public:
	Visualisation_Splinters();

	void activate();
	void deactivate();

	void processSamples(const Buffer& buf, unsigned samples);
	void renderFrame();
	void drawInterface();
	std::string titleString() override { return "Splinters"; };

private:

	Shader* m_objectShader;

	std::vector<SplinterNode*> m_roots;


	glm::vec3 abc;
	glm::vec3 abcOrig;
	Mesh m_testMesh;

	long long m_seed;
};