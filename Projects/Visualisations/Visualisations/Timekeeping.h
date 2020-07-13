#pragma once
#include <chrono>

using Duration = std::chrono::duration<double>;
using Timepoint = std::chrono::time_point<std::chrono::steady_clock, Duration>;
struct Timestep
{
	Timestep(Timepoint prev, Timepoint current, Timepoint first)
		: m_previous{ prev }, m_current{ current }, m_first{ first }
	{}
	auto dt() const { return m_current - m_previous; }
	auto current() const { return m_current - m_first; }
	auto last() const { return m_previous - m_first; }
	const Timepoint m_previous, m_current, m_first;
};