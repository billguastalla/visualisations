#include "GameOfLife_Game.h"
#include "GameOfLife_Grid.h"
#include "GameOfLife_Element.h"


#include "../MetaDistribution.h"

#include <algorithm>
#include <thread>
#include <random>

Game::Game(const std::vector<int> & dimensions)
{
	m_original = new Grid(dimensions);
	m_current = new Grid(m_original);

	m_rules = std::vector<std::pair<bool, bool>>();
	for (int i = 0; i < m_current->getNeighbourCount(); ++i)
	{
		std::pair<bool, bool> rule;
		rule.first = false;
		rule.second = false;
		m_rules.push_back(rule);
	}

	// Initially, all elements are active.
	for (int index = 0; index < m_current->totalElements(); ++index)
	{
		m_activeElements.push_back(index);
	}
}

void Game::nextTurn()
{
	*m_original = *m_current;

	auto calcThread = [this](int start, int end) {
		for (int index = start; index < end; ++index)
		{
			Element * currentElement = m_current->operator[](index);
			Element * originalElement = m_original->operator[](index);
			int aliveNeighbours = 0;
			// Start at neighbour = 1 because neighbour 0 is the element at that index rather than its neighbours.
			for (unsigned int neighbour = 1; neighbour < originalElement->neighbours->size(); ++neighbour)
			{
				if (originalElement->neighbours->operator[](neighbour)->alive)
				{
					++aliveNeighbours;
				}
			}
			if (!currentElement->alive)
				currentElement->alive = m_rules.at(aliveNeighbours).first;
			else
				currentElement->alive = m_rules.at(aliveNeighbours).second;
		}};

	size_t total = currentGrid()->totalElements();
	int endOne = (int)std::floor((float)total / 4);
	int endTwo = (int)std::floor((float)total * 2 / 4);
	int endThree = (int)std::floor((float)total * 3 / 4);

	std::thread one(calcThread, 0, endOne);
	std::thread two(calcThread, endOne, endTwo);
	std::thread three(calcThread, endTwo, endThree);
	std::thread four(calcThread, endThree, (int)total);
	one.join();
	two.join();
	three.join();
	four.join();
}

// Need to think about this thoroughly (UNFINISHED)
/*
	1. Equality operator for element's neighbours.
	2. Maintained list of active elements.
	3. Highest efficiency ordered insertion operator into list..
	4. Likewise for removeAt()..
	5. If neighbours identical -> find & remove from list. If neighbours different.. add to list along with all neighbours..
*/
void Game::nextTurnOptimised()
{
	*m_original = *m_current;
}

void Game::randomRuleset()
{
	std::mt19937_64 r{};
	std::normal_distribution<double> startingDist{ 0.5,1.0 };
	MetaDistribution<double> metaDistLeft{ startingDist(r),1.0,1.0,1.0 };
	MetaDistribution<double> metaDistRight{ startingDist(r),1.0,1.0,1.0 };

	double neighbours{ (double)m_current->getNeighbourCount() };
	double current{ 0.0 };

	std::normal_distribution<double> distLeft{ metaDistLeft.createDistribution() };
	std::normal_distribution<double> distRight{ metaDistRight.createDistribution() };
	for (auto i = m_rules.begin(); i != m_rules.end(); ++i)
	{
		i->first = (distLeft(r) > ((neighbours - current) / neighbours));
		i->second = (distRight(r) > ((/*neighbours - */current) / neighbours));
		current++;
	}
	m_rules[0].first = false;
	m_rules[0].second = false;
}

void Game::setRule(int row, std::pair<bool, bool> rule)
{
	m_rules[row] = rule;
}