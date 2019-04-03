#include "GameOfLife_Element.h"

// Warning: Makes no attempt to check that the neighbourhood type is the same across elements/grids.
bool Element::neighbourhoodIdentical(Element & element)
{
	for(int nIndex = 0; nIndex < element.neighbours->size(); ++nIndex)
		if(element.neighbours->operator[](nIndex)->alive != neighbours->operator[](nIndex)->alive)
			return false;
	return true;
}
