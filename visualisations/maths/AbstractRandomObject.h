#pragma once
#include <random>
class AbstractRandomObject
{
public:
	AbstractRandomObject() :
		m_randomDevice{},
		m_mersenneTwister{ m_randomDevice() }
	{};
	AbstractRandomObject(AbstractRandomObject & other):
		m_randomDevice{},
		m_mersenneTwister{ m_randomDevice() }
	{};
	~AbstractRandomObject() {};

	std::random_device & randDevice() { return m_randomDevice; }
	std::mt19937 & mersenneTwister() { return m_mersenneTwister; }
protected:
	std::random_device m_randomDevice;
	std::mt19937 m_mersenneTwister;
};