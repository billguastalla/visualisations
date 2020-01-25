#pragma once
#include <vector>

class Node;

/*
	-> Most poignant question here is how do we wind a tree with a continuous mesh?
		\ /
		 |
		-> Two holes should appear in the splitting of a branch.
		-> We need to change the way that indices are written, at the point of splitting.
			-> There appears to need to be a connector model, which has one hole for the base,
				and N holes for the branches. (upside down pants)
				-> The connector needs to know the angles phi and theta of the branches.
					(possibly also their radius)
					-> there appear to be semi-circles joining opposite sides of the base
						circle, which split the branches.
					-> We want to control the resolution of the connector.
					-> We want to pass an ellipsoid shape into a branch, so that the
						branch mesh can smoothly wind the ellipsoid into a cylinder.
*/
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

