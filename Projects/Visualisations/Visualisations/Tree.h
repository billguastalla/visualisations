#pragma once
#include <GLM/glm.hpp>
#include <vector>

class Node;

class Tree
{
	Tree(unsigned depth = 0u, unsigned nodesPerLayer = 1u);

private:
	Node* m_rootItem;
};

class Node
{
public:
	Node(Node* parent = nullptr);

	void addChildren(unsigned depth = 0u, unsigned nodesPerLayer = 1u);
private:
	double m_distAlongParent; // Position of start of branch along parent takes values 0 to 1, where it branches off the parent from.
	double m_length;		  // Length of branch
	/* Orientation the tree makes with the plane that is normal
		to the direction of the parent node, */ // This isn't enough information! Basis can be chosen arbitrarily!
	double m_theta, m_phi;

	Node* p_parent;
	std::vector<Node*> m_children;
};

