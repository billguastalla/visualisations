#pragma once

#include <chrono>
#include <ctime>
#include <iomanip>
#include <sstream>

#pragma warning(disable : 4996)
auto getTimeStr = []()
{
	std::time_t t = std::time(nullptr);
	std::tm tm = *std::gmtime(&t);
	std::stringstream dtSStream{};
	dtSStream << std::put_time(&tm, "%Y%m%d%H%M%S");
	return dtSStream.str();
};
