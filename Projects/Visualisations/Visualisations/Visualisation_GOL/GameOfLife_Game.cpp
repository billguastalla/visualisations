#include "GameOfLife_Game.h"
#include "GameOfLife_Grid.h"
#include "GameOfLife_Element.h"


#include "../MetaDistribution.h"

#include <algorithm>
#include <thread>
#include <random>

Game::Game(const std::vector<int>& dimensions)
	:
	m_original{dimensions},
	m_current{m_original}
{
	m_rules = std::vector<std::pair<bool, bool>>();
	m_rules.resize(m_current.getNeighbourCount(), std::pair<bool, bool>{ false,false });

	for (int index = 0; index < m_current.totalElements(); ++index)
		m_activeElements.push_back(index);
}

void Game::nextTurn()
{
	m_original = m_current;

	auto calcThread = [this](int start, int end) {
		for (int index = start; index < end; ++index)
		{
			Element* currentElement = m_current[index];
			Element* originalElement = m_original[index];
			int aliveNeighbours = 0;
			for (unsigned int neighbour = 1; neighbour < originalElement->neighbours.size(); ++neighbour)
			{
				if (originalElement->neighbours[neighbour]->alive)
				{
					++aliveNeighbours;
				}
			}
			if (!currentElement->alive)
				currentElement->alive = m_rules.at(aliveNeighbours).first;
			else
				currentElement->alive = m_rules.at(aliveNeighbours).second;
		}};

	size_t total = currentGrid().totalElements();
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

void Game::nextTurnOptimised()
{
	m_original = m_current;
}

void Game::randomRuleset()
{
	std::mt19937_64 r{};
	std::normal_distribution<double> startingDist{ 0.5,1.0 };
	MetaDistribution<double> metaDistLeft{ startingDist(r),1.0,1.0,1.0 };
	MetaDistribution<double> metaDistRight{ startingDist(r),1.0,1.0,1.0 };

	double neighbours{ (double)m_current.getNeighbourCount() };
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

void Game::setRules(const std::vector<std::pair<bool, bool>>& rules)
{
	m_rules = rules;
}

void Game::setDimensions(const std::vector<int>& dimensions)
{
	m_original = Grid{ dimensions };
	m_current = Grid{ m_original };
	m_activeElements.clear();
	for (int index = 0; index < m_current.totalElements(); ++index)
		m_activeElements.push_back(index);

}
