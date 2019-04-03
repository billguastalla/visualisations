#include <vector>
#include "GameOfLife_Element.h"

class ElementList : public std::vector<Element>
{
public:
	ElementList();
	ElementList(int & count);

	void setAlive(unsigned int & index, bool alive = true);

	// Cell counter:
private:
	unsigned int m_aliveCount;
public:
	unsigned int getAliveCount();
	// -------------
};
