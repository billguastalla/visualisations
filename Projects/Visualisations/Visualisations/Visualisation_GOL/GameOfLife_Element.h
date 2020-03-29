#pragma once
#include <vector>

struct Element
{
	Element()
		:
		active{true},
		alive{false},
		neighbours{nullptr},
		index{0}
	{}
	Element operator=(Element & element)
	{
		active = element.active; alive = element.alive;
		return *this;
	}
	bool active;
	bool alive;
	std::vector<Element*> neighbours;
	// This is temporary: (ha, clearly not)
	int index;
};
