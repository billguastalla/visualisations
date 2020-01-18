#pragma once
#include <vector>

struct Element
{
	Element()
		:
		active{true},
		alive{ false },
		neighbours{nullptr},
		index{0}
	{}
	/*
		The deep copy may never need to be used, but is just a reminder of the actual functionality of the copy assignment operator.
			-> The normal operator=() ignores the neighbours, and relies on them having been set by the grid that holds the elements. Hence also ignores indexing..
			-> The deep copy with neighbours actually copies the pointers of the original, such that the copy now eternally relies on the states
			   of the original's neighbours rather than its own... This is a potential memory leak...
			tl;dr : the operator=() does the absolute minimum and relies on the Elements already being part of a grid, deep copy literally copies all contents.
	*/
	Element operator=(Element & element)
	{
		active = element.active; alive = element.alive;
		return *this;
	}
	Element deepCopyWithNeighbours(Element & element)
	{
		active = element.active; alive = element.alive;
		index = element.index;
		neighbours = element.neighbours;
	}
	bool neighbourhoodIdentical(Element & element);

	bool active;
	bool alive;
	std::vector<Element*> * neighbours;

	// This is temporary: (ha, clearly not)
	int index;
};
