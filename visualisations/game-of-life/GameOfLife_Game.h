/* This code is from 2015, and so has some old design considerations */

// Basically, the algorithm should run through games of life with different rules.
/*
- The grid should have random initial starting states.
- The alive bool is for whether it is on or off.
- The active variable is for whether its state should be calculated in the next generation.

- Algorithm should run a certain set of rules three different ways:
- One where it considers blocks that stay white with white around them inactive.
- Another where it considers blocks that stay black with black around them inactive.
- Finally where it calculates the future states of all blocks regardless.

- > Would compare computation time and see if there are situations in which it is more effective to calculate all blocks. 

- - > Otherwise in future try neural network to create training set of interesting results etc..
*/

#include <vector>

#include "GameOfLife_Grid.h"

class Game
{
public:

	Game(const std::vector<int> & dimensions);
	Grid & currentGrid() {return m_current;}
	std::vector<std::pair<bool,bool>> & getRules(){ return m_rules; }


	void setRule(int row, std::pair<bool, bool> rule);
	void setRules(const std::vector<std::pair<bool, bool>>& rules);
	
	std::vector<int> dimensions() const { return m_current.dimensions(); }
	void setDimensions(const std::vector<int>& dimensions);

	void nextTurn();

	void nextTurnOptimised();
	void randomRuleset();
private:
	std::vector<int> m_activeElements;

	Grid m_original;
	Grid m_current;

	std::vector<std::pair<bool,bool>> m_rules;
};

