#pragma once
#include "GameOfLife_Element.h"
#include <vector>
#include <ctime>

struct Edges
{
	// first is the axis, and second is the index.
	// This forms a plus in n dimensions. So the vector length will be dimensions*2.
	struct EdgePair
	{
		typedef int Axis;
		struct AxisSide
		{
			int top;
			int bottom;
		};
		Axis axis;
		AxisSide sideIndex;
	};

	std::vector<EdgePair> axisIndices;
};

class Grid
{
public:
	enum class NeighbourType
	{
		Moore,
		Plus,
		Diamond,
		StraightAxis,
		DiagonalAxis
	};
	Grid(const std::vector<int>& dimensions, const NeighbourType gridNeighbourType = NeighbourType::Moore);
	Grid(Grid* other);

	size_t totalElements() { return m_grid.size(); }
	size_t getNoOfDimensions() { return m_dimensions.size(); }
	const int getDimensionLength(int axis) { return m_dimensions[axis]; }
	const int getNeighbourCount() { return m_neighbourCount; }

	Element* operator[](int index) { return &m_grid[index]; }

	const std::vector<int>& dimensions() const { return m_dimensions; }


	//std::vector<int> getMooreNeighbours(int index)
	//{
	//	int size = m_grid.size();
	//	
	//	std::vector<int> result{};

	//	// Find if the current dimension we're working in is an axis that the current element is at the edge of.
	//	for (int axis = 0; axis < dimensions; ++axis)
	//	{
	//		std::vector<int> topNeighbours = getMooreNeighbours(m_gridEdges[index].axisIndices[axis].sideIndex.top, m_gridEdges[index].axisIndices[axis].axis);
	//		std::vector<int> bottomNeighbours = getMooreNeighbours(m_gridEdges[index].axisIndices[axis].sideIndex.bottom, m_gridEdges[index].axisIndices[axis].axis);
	//		neighbouringCells.insert(neighbouringCells.end(), topNeighbours.begin(), topNeighbours.end());
	//		neighbouringCells.insert(neighbouringCells.end(), bottomNeighbours.begin(), bottomNeighbours.end());
	//	}

	//}
	Edges getEdges(int index);

	/* */
	void insertPrimitive(int extent = 2);

	std::vector<Element*> getMooreNeighbours(int index, int dimensions);
	std::vector<Element*> getPlusNeighbours(int index, int dimensions);
	std::vector<Element*> getDiamondNeighbours(int index, int dimensions);


private:
	// The actual list of elements
	std::vector<Element> m_grid;
	// The array of lengths in each dimension
	std::vector<int> m_dimensions;


	NeighbourType m_neighbourType;
	int m_neighbourCount;

	// This will be for the optimisation. Will hold onto whichever elements have activity. (Part of a game not a grid?)
	std::vector<Element*> m_elementsToEvaluateNextTurn;

	void allocateNeighbours();

	std::vector<Edges> m_gridEdges;

private:
	// This is the index difference between adjacent elements on a certain axis.
	int indexDistance(int dimensions);

public:
	// Not sure how to use these yet but just deduced from general array formula, and may be useful to be able to move between them..
	int index(std::vector<int> coords);
	std::vector<int> coordinates(int index) const;
	std::vector<std::vector<int>> aliveCoords() const;

	void clear();
	void setRandAlive(int probability);
	void addCell(int type);

	std::clock_t m_timer;
};
