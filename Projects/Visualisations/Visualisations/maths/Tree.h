#pragma once
#include <GLM/glm.hpp>
#include <vector>
#include <random>

class Tree; 

class Node
{
public:
	Node(Node* parent = nullptr);

	void addChildren(Tree* t, unsigned depth = 0u, unsigned nodesPerLayer = 1u, int seed = 0);
	void clear();
	glm::vec3 base() const;
	glm::vec3 m_branchArrow; // work in cartesian coordinates
	double m_branchPos; // position of start of branch along parent takes values 0 to 1, where it branches off the parent from.

	std::vector<glm::vec3> vertices(glm::vec3 parentBase, glm::vec3 parentArrow = glm::vec3{0.f}); // top caller uses default argument & chooses position for base of tree.
	Node* p_parent;
	std::vector<Node*> m_children;
};


class Tree
{
public:
	Tree();
	void build(unsigned depth = 0u, unsigned nodesPerLayer = 1u);
	void setPositions();

	void clear();
	const std::vector<glm::vec3>& vertices() const;
	std::vector<Node*> m_nodes;
private:
	mutable std::vector<glm::vec3> m_vertexCache;
};
