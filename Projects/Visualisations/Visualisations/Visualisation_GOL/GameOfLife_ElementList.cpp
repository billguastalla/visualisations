#include "GameOfLife_ElementList.h"

ElementList::ElementList()
	:
	std::vector<Element>{},
	m_aliveCount{0}
{
}

ElementList::ElementList(int & count)
	:
	std::vector<Element>{count}
{
	// Note:	Relies on constructor of Element defaulting as alive = false.
	m_aliveCount = 0;
}

unsigned int ElementList::getAliveCount()
{
	return m_aliveCount;
}

void ElementList::setAlive(unsigned int & index, bool alive)
{
	if(operator[](index).alive)
	{
		if(!alive)
		{
			m_aliveCount--;
			operator[](index).alive = false;
		}
	}
	else
	{
		if(alive)
		{
			m_aliveCount++;
			operator[](index).alive = true;
		}
	}
}