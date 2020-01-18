#pragma once
#include <vector>

class Node;

class Tree
{

private:
	Node* m_rootItem;
};

class Node
{
private:
	double m_length;
	/* Orientation the tree makes with the plane that is normal
		to the direction of the parent node, */
	double m_theta, m_phi;

	Node* p_parent;
	std::vector<Node*> m_children;
};

