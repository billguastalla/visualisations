#include "Tree.h"

Tree::Tree(unsigned depth, unsigned nodesPerLayer)
	:
	m_rootItem{ new Node{nullptr} }
{
	m_rootItem->addChildren(depth, nodesPerLayer);
}

Node::Node(Node* parent)
	:
	p_parent{ parent },
	m_children{},
	m_distAlongParent{ 0. },
	m_length{ 1. },
	m_phi{ 0. },
	m_theta{ 0. }
{

}

void Node::addChildren(unsigned depth, unsigned nodesPerLayer)
{
	for (unsigned i = 0u; i < nodesPerLayer; ++i)
	{
		Node* child{ this };
		m_children.push_back(child);
		if (depth >= 0u)
			addChildren(--depth, nodesPerLayer);
	}
}
