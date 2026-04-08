#pragma once


#include "float.hpp"
#include "int.hpp"


static U32 seed = 0;


// Get a number in the range [0, 1)
static Float random()
{
	// Some randomly chosen numbers
	constexpr U32 RANDOM_A = 7983801;
	constexpr U32 RANDOM_B = 825103368;

	// 4294967553 is the first Float `x` that satisfies `U32_MAX / x < 1`
	constexpr Float RANGE_DIVISOR = 1.0 / 4294967553.0;

	seed *= RANDOM_A;
	seed += RANDOM_B;

	return (Float) seed * RANGE_DIVISOR;
}


// Get a number in the given range
static Float random(Float min, Float max)
{
	return min + (max - min) * random();
}
