#pragma once


#include "float.hpp"


struct Interval
{
	Float min;
	Float max;

	Interval() = default;

	constexpr Interval(Float min_, Float max_): min(min_), max(max_)
	{}

	constexpr Float clamp(Float value) const
	{
		if (value < min)
			return min;
		if (value > max)
			return max;
		return value;
	}

	constexpr bool contains(Float value) const
	{
		return min <= value and value <= max;
	}

	constexpr Float size() const
	{
		return max - min;
	}

	constexpr bool surrounds(Float value) const
	{
		return min < value and value < max;
	}
};


constexpr Interval EMPTY(+INFINITY, -INFINITY);

constexpr Interval UNIVERSE(-INFINITY, +INFINITY);
