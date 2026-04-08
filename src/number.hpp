#pragma once


template <typename T>
constexpr T absolute_value(T x)
{
	return x < 0 ? -x : x;
}


template <typename T>
constexpr T maximum(T x, T y)
{
	return x > y ? x : y;
}


template <typename T>
constexpr T minimum(T x, T y)
{
	return x < y ? x : y;
}
