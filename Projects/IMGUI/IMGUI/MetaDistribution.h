#pragma once
#include "AbstractRandomObject.h"
#include <random>

template<class T>
/* First is mean, Second is sigma */
class MetaDistribution : public std::pair<std::normal_distribution<T>, std::normal_distribution<T>>
{
public:
	MetaDistribution() {};
	MetaDistribution(T mean, T sigma, T sigmaMean, T sigmaSigma)
	{
		/* TODO: Think about swapping 3rd & 4th args for doubles and generating % deviation on mean & sigma. */
		first = std::normal_distribution<T>{ mean, sigmaMean };
		second = std::normal_distribution<T>{ sigma, sigmaSigma };
	};
	std::normal_distribution<T> createDistribution()
	{
		AbstractRandomObject obj{};
		T left = std::abs(first(obj.randDevice()));
		T right = std::abs(second(obj.randDevice()));
		std::normal_distribution<T> result{left,right};
		return result;
	}
	std::normal_distribution<T> mean() const { return first; }
	std::normal_distribution<T> sigma() const { return second; }
	void setMean(std::normal_distribution<T> mean) { first = mean; }
	void setSigma(std::normal_distribution<T> sigma) { second = sigma; }
};
