#include "GameOfLife_Grid.h"

#include <iostream>
#include <string>
#include <functional>
//
//
//struct elems /* multiples of 64 only */
//{
//	/*
//		1. Build neighbourhood
//	*/
//	elems buildHeatmap()
//	{
//		std::vector<uint8_t> result;
//		result.resize(m_elems.size() * 64u,0u);
//		for (int i = 0; i < result.size(); ++i)
//		{
//			std::div_t forward{ i + 1,64 };
//			std::div_t backward{ i - 1,64 };
//			result[i] += m_elems[d.quot] << d.rem;
//		}
//
//	}
//	std::vector<int64_t> m_elems;
//	std::vector<std::div_t> m_neighbours;
//
//
//	std::vector<int> dimensions;
//};
//
//struct gm
//{
//	elems m_cells;
//	elems m_heatmap;
//};




Grid::Grid(const std::vector<int> & dimensions, Grid::NeighbourType neighbourType)
	:
	m_dimensions{dimensions},
	m_neighbourType{neighbourType},

	m_elementsToEvaluateNextTurn{},
	m_grid{},
	m_gridEdges{},
	m_neighbourCount{},
	m_timer{clock()}
{

	int numberOfElements = indexDistance(dimensions.size());
	m_grid.resize(numberOfElements);
	for (int index = 0; index < numberOfElements; ++index)
	{
		m_grid[index].active = true;
		m_grid[index].alive = false;

		m_grid[index].index = index;
	}

	m_timer = clock() - m_timer;
	std::cout << "\tGrid - Making elements and setting them to alive/active took: " << (double)m_timer / (double)CLOCKS_PER_SEC << "s" << std::endl;
	m_timer = clock();

	m_gridEdges.resize(numberOfElements);
	for (int index = 0; index < numberOfElements; ++index)
		m_gridEdges[index] = getEdges(index);

	m_timer = clock() - m_timer;
	std::cout << "\tGrid - Calculating the edges for each element took: " << (double)m_timer / (double)CLOCKS_PER_SEC << "s" << std::endl;
	m_timer = clock();


	/*
	Only way to speed this up (other than x64) is to thread.
	-> Need to divide area into 4/8 regions, such that every cell is guaranteed not to share a neighbour with one from another region.
	-> Then run allocateNeighbours(Region) in 4/8 threads.
	-> Then need to fill in the boundaries in the main() thread.
	*/
	allocateNeighbours();
	m_timer = clock() - m_timer;
	std::cout << "\tGrid - Assigning neighbours for each element took: " << (double)m_timer / (double)CLOCKS_PER_SEC << "s" << std::endl;

}

Grid::Grid(Grid * other)
{
	m_dimensions = other->m_dimensions;
	m_neighbourType = other->m_neighbourType;
	m_neighbourCount = other->m_neighbourCount;
	m_grid = other->m_grid;
	m_gridEdges = other->m_gridEdges;
	m_elementsToEvaluateNextTurn = other->m_elementsToEvaluateNextTurn;
}

void Grid::allocateNeighbours()
{
	// Here we'll find and populate the pointers to the neighbours for each element
	int noOfDimensions = getNoOfDimensions();
	switch (m_neighbourType)
	{
	case NeighbourType::Moore:
		// Warning: this is a neighbourhood system where neighbours include themselves..
		m_neighbourCount = pow(3, getNoOfDimensions());
		for (int index = 0; index < totalElements(); ++index)
			m_grid[index].neighbours = getMooreNeighbours(index, noOfDimensions);
		break;
	case NeighbourType::Plus:
		// Warning: this is a neighbourhood system where neighbours include themselves..
		m_neighbourCount = (2 * getNoOfDimensions()) + 1;
		for (int index = 0; index < totalElements(); ++index)
			m_grid[index].neighbours = getPlusNeighbours(index, noOfDimensions);
		break;
	case NeighbourType::Diamond:
		m_neighbourCount = pow(2, getNoOfDimensions());
		for (int index = 0; index < totalElements(); ++index)
			m_grid[index].neighbours = getDiamondNeighbours(index, noOfDimensions);
		break;
	default:
		break;
	}
}

Edges Grid::getEdges(int index)
{
	// Divides by each dimension and gets the result and remainder.
	// This is used to see if an element is at the edge of the N-dimensional shape.
	// Then the edges are a list of axes at which the element is an edge, and whether it is at the top (true) or bottom (false) of that axis.
	std::vector<std::div_t> edgeRemainders;
	Edges edges;
	int dividedIndex = index;

	for (int axis = 0; axis < m_dimensions.size(); ++axis)
	{
		edgeRemainders.push_back(std::div(dividedIndex, m_dimensions[axis]));
		dividedIndex = edgeRemainders[axis].quot;
		Edges::EdgePair edgePair;
		edgePair.axis = axis;
		// If there's no remainder in this axis, then it is at the BOTTOM edge of that axis.
		if (edgeRemainders[axis].rem == 0)
		{
			edgePair.sideIndex.bottom = index + indexDistance(axis);
			edgePair.sideIndex.top = index + (indexDistance(axis + 1) - indexDistance(axis));
		}
		// If the remainder in the axis is one less than the width of the dimension, then it is at the TOP edge of that axis.
		else if (edgeRemainders[axis].rem == (m_dimensions[axis] - 1))
		{
			edgePair.sideIndex.top = index - indexDistance(axis);
			edgePair.sideIndex.bottom = index - (indexDistance(axis + 1) - indexDistance(axis));
		}
		// Otherwise, the element is NOT at either edge of the given axis.
		else
		{
			edgePair.sideIndex.top = index + indexDistance(axis);
			edgePair.sideIndex.bottom = index - indexDistance(axis);
		}
		edges.axisIndices.push_back(edgePair);
	}

	return edges;
}
#include <random>

/* WARNING: This is O(N^N) */
void Grid::insertPrimitive(int extent)
{

	/* General idea here is to walk along neighbourhoods of neighbourhoods of neighbourhoods etc, randomly enabling elements.
		-> The neighbourhoods of enabled elements are used to find new elements. */


	/* Randomly enable the neighbours of an element according to a distribution. */
	auto enableElemNeighbours = [this](Element * elem, std::normal_distribution<double> & d, std::mt19937_64 mersene)
	{
		/* Problem was that we are referring to the other grid when we call elem->neighbours. This is bad design from 2015. */
		//for (auto e = (*elem->neighbours).begin(); e != (*elem->neighbours).end(); ++e)
		//	if (d(mersene) > 0.0)
		//	{
		//		(*e)->alive = true;
		//		res.push_back(*e);
		//	}
		std::vector<Element*> neighbours{ getMooreNeighbours(elem->index, dimensions().size()) };
		std::vector<Element*> res{};
		for (auto e = neighbours.begin(); e != neighbours.end(); ++e)
			if (d(mersene) > 0.0)
			{
				(*e)->alive = true;
				res.push_back(*e);
			}
		return res; /* Return only neighbours you enabled.*/
	};

	std::function<std::vector<Element*>(int, std::vector<Element*> &, std::normal_distribution<double> &, std::mt19937_64)> buildNeighbourSeq;
	buildNeighbourSeq = [enableElemNeighbours, &buildNeighbourSeq](int depth, std::vector<Element*> & elem, std::normal_distribution<double> & d, std::mt19937_64 mersene) -> std::vector<Element*>
	{ /* From a list of elements, enable all neighbours optionally, and then add enabled to a new list. */
		std::vector<Element*> res;
		if (depth <= 0) return res;
		for (auto e = elem.begin(); e != elem.end(); ++e)
		{
			std::vector<Element*> enabledNeighbours{ enableElemNeighbours((*e),d,mersene) };
			res.insert(std::end(res), std::begin(enabledNeighbours), std::end(enabledNeighbours));
		}
		if (depth > 0)
		{
			std::vector<Element*> neighs = buildNeighbourSeq(--depth, res, d, mersene);
			res.insert(std::end(res), std::begin(neighs), std::end(neighs));
		}
		return res;
	};

	std::normal_distribution<double> dist{ -0.5,1.0 };
	std::mt19937_64 mt;

	size_t s = m_grid.size();

	std::vector<int> middleCoords{ m_dimensions };
	for (auto i = middleCoords.begin(); i != middleCoords.end(); ++i)
		*i /= 2;
	int middleIdx{ index(middleCoords) };
	Element * e =  (middleIdx >= 0 && middleIdx < m_grid.size()) ? &m_grid[middleIdx] : &m_grid[0];
	std::vector<Element*> el{ e };
	el = buildNeighbourSeq(extent, el, dist, mt);


	//		std::vector<Element*> el = getMooreNeighbours(maxSize / 2u, m_dimensions.size());
}

std::vector<Element*> Grid::getMooreNeighbours(int index, int dimensions)
{
	if (dimensions < 0)
		return std::vector<Element*>{};

	std::vector<Element*> neighbouringCells;
	// Add the element at the current index
	neighbouringCells.push_back(&m_grid[index]);

	// Find if the current dimension we're working in is an axis that the current element is at the edge of.
	for (int axis = 0; axis < dimensions; ++axis)
	{
		std::vector<Element*> topNeighbours = getMooreNeighbours(m_gridEdges[index].axisIndices[axis].sideIndex.top, m_gridEdges[index].axisIndices[axis].axis);
		std::vector<Element*> bottomNeighbours = getMooreNeighbours(m_gridEdges[index].axisIndices[axis].sideIndex.bottom, m_gridEdges[index].axisIndices[axis].axis);
		neighbouringCells.insert(neighbouringCells.end(), topNeighbours.begin(), topNeighbours.end());
		neighbouringCells.insert(neighbouringCells.end(), bottomNeighbours.begin(), bottomNeighbours.end());
	}
	return neighbouringCells;
}

std::vector<Element*> Grid::getPlusNeighbours(int index, int dimensions)
{
	std::vector<Element*> neighbouringCells;
	neighbouringCells.push_back(&m_grid[index]);
	for (int axis = 0; axis < dimensions; ++axis)
	{
		Element * topNeighbour = &m_grid[m_gridEdges[index].axisIndices[axis].sideIndex.top];
		Element * bottomNeighbour = &m_grid[m_gridEdges[index].axisIndices[axis].sideIndex.bottom];
		neighbouringCells.push_back(topNeighbour);
		neighbouringCells.push_back(bottomNeighbour);
	}
	return neighbouringCells;
}

std::vector<Element*> Grid::getDiamondNeighbours(int index, int dimensions)
{
	if (dimensions < 0)
		return std::vector<Element*>{};

	std::vector<Element*> neighbouringCells;
	// Add the element at the current index
	if (dimensions == 0)
	{
		neighbouringCells.push_back(&m_grid[index]);
		return neighbouringCells;
	}
	std::vector<Element*> topNeighbours = getDiamondNeighbours(m_gridEdges[index].axisIndices[dimensions - 1].sideIndex.bottom, dimensions - 1);
	std::vector<Element*> bottomNeighbours = getDiamondNeighbours(m_gridEdges[index].axisIndices[dimensions - 1].sideIndex.top, dimensions - 1);
	neighbouringCells.insert(neighbouringCells.end(), topNeighbours.begin(), topNeighbours.end());
	neighbouringCells.insert(neighbouringCells.end(), bottomNeighbours.begin(), bottomNeighbours.end());
	// Find if the current dimension we're working in is an axis that the current element is at the edge of.
	return neighbouringCells;
}

// i.e. in 3d if you want the adjacent element that is +1 in the z direction, the number of elements away is length*width away.
int Grid::indexDistance(int dimensions)
{
	int dist = 1;
	for (int i = 0; i < dimensions; ++i)
		dist *= m_dimensions[i];
	return dist;
}

int Grid::index(std::vector<int> coords)
{
	int index = 0;
	for (int axis = 0; axis < coords.size(); ++axis)
		index += coords[axis] * indexDistance(axis);
	return index;
}

std::vector<int> Grid::coordinates(int index) const
{
	std::vector<int> coordList;
	std::div_t indexRemainder;
	int divisionResult = index;
	for (int axis = 0; axis < m_dimensions.size(); ++axis)
	{
		indexRemainder = std::div(divisionResult, m_dimensions[axis]);
		divisionResult = indexRemainder.quot;
		coordList.push_back(indexRemainder.rem);
	}
	return coordList;
}

std::vector<std::vector<int>> Grid::aliveCoords() const
{
	std::vector<std::vector<int>> result{};
	for (auto i = m_grid.begin(); i != m_grid.end(); ++i)
		if (i->alive)
			result.push_back(coordinates(i->index));
	return result;
}

void Grid::clear()
{
	for (long int i = 0; i < totalElements(); ++i)
		m_grid[i].alive = false;
}

void Grid::setRandAlive(int probablity)
{
	for (long int i = 0; i < totalElements(); ++i)
		m_grid[i].alive = ((rand() % probablity) == 0);
}

void Grid::addCell(int type)
{
	_ASSERT(getNoOfDimensions() == 3);
	std::vector<int> coords;
	coords.resize(3);
	int half[3];

	for (int i = 0; i < m_dimensions.size(); ++i)
	{
		if (getDimensionLength(i) % 2 != 0)
		{
			half[i] = std::floor(getDimensionLength(0) / 2);
		}
		else
		{
			half[i] = ((getDimensionLength(i) / 2) - 1);
		}
	}

	switch (type)
	{
	case 1: // Cube
		coords[0] = half[0] + 0;
		coords[1] = half[1] + 0;
		coords[2] = half[2] + 0;
		this->operator[](index(coords))->alive = true;
		coords[0] = half[0] + 1;
		this->operator[](index(coords))->alive = true;
		coords[0] = half[0] + 0;
		coords[1] = half[1] + 1;
		this->operator[](index(coords))->alive = true;
		coords[0] = half[0] + 1;
		coords[1] = half[1] + 1;
		this->operator[](index(coords))->alive = true;
		coords[0] = half[0] + 0;
		coords[1] = half[1] + 0;
		coords[2] = half[2] + 1;
		this->operator[](index(coords))->alive = true;
		coords[0] = half[0] + 1;
		coords[1] = half[1] + 0;
		coords[2] = half[2] + 1;
		this->operator[](index(coords))->alive = true;
		coords[0] = half[0] + 0;
		coords[1] = half[1] + 1;
		coords[2] = half[2] + 1;
		this->operator[](index(coords))->alive = true;
		coords[0] = half[0] + 1;
		coords[1] = half[1] + 1;
		coords[2] = half[2] + 1;
		this->operator[](index(coords))->alive = true;

		return;
	default:
		return;
	}
}