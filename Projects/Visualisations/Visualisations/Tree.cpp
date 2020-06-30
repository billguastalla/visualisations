#include "Tree.h"
// stack overflow on using distributions in recursive functions.
// lesson: only use recursion when necessary. can build properties in the tree
// without packing up the stack.

Tree::Tree()
	:
	m_nodes{}
{
}

Node::Node(Node* parent)
	:
	p_parent{ parent },
	m_children{},
	m_branchPos{ 0. },
	m_branchArrow{ 0.f,1.f,0.f }
{

}
void Tree::build(unsigned depth, unsigned nodesPerLayer)
{
	clear();
	m_nodes.push_back(new Node{ nullptr });
	m_nodes[0]->addChildren(this, depth, nodesPerLayer, 0u);
	setPositions();
}

void Tree::setPositions()
{
	std::uniform_real_distribution<float> distLY{ 0, 1.0 };
	std::uniform_real_distribution<float> distZX{ 0, 1.0 };
	std::normal_distribution<float> branchScale{ 0.6,0.1 };
	std::random_device rd{};
	std::mt19937 gen{ rd() };
	for (Node* n : m_nodes)
	{
		n->m_branchArrow = branchScale(gen) * glm::normalize(glm::vec3{ distZX(gen),distLY(gen),distZX(gen) });
		n->m_branchPos = distLY(gen);
	}
	m_vertexCache.clear();
}

void Tree::clear()
{
	for (Node* n : m_nodes)
		delete n;
	m_nodes.clear();
	m_vertexCache.clear();
}

const std::vector<glm::vec3> & Tree::vertices() const
{
	if (m_vertexCache.empty())
	{
		for (Node* n : m_nodes)
		{
			glm::vec3 base{ n->base() };
			glm::vec3 arrow{ n->m_branchArrow };
			for (int i = 0; i < 10; ++i)
			{
				float interpolation{ (float)i / 10.f };
				m_vertexCache.push_back(base + (arrow * interpolation));
			}
		}
	}
	return m_vertexCache;
}

void Node::addChildren(Tree* t, unsigned depth, unsigned nodesPerLayer, int seed)
{
	--depth;
	for (unsigned i = 0u; i < nodesPerLayer; ++i)
	{
		Node* child{ new Node{this} };
		m_children.push_back(child);
		t->m_nodes.push_back(child);
		if (depth > 1u)
			child->addChildren(t, depth, nodesPerLayer);
	}
}

void Node::clear()
{
	for (Node* n : m_children)
		n->clear();
	for (Node* n : m_children)
		delete n;
	m_children.clear();
}

glm::vec3 Node::base() const
{
	glm::vec3 result{ 0.f };
	const Node* n{ this };
	while (n->p_parent != nullptr)
	{
		result += (n->p_parent->m_branchArrow * (float)n->m_branchPos);
		n = n->p_parent;
	}
	return result;
}