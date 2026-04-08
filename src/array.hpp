#pragma once


#include "int.hpp"


// A statically-allocated contiguous collection of elements
template <typename T, Size S>
struct Array
{
	T data[S];

	constexpr const T* begin() const
	{
		return data;
	}

	constexpr T* begin()
	{
		return data;
	}

	constexpr const T* end() const
	{
		return data + S;
	}

	constexpr T* end()
	{
		return data + S;
	}

	// * SAFETY: index must be less than size
	constexpr const T& operator[](Size index) const
	{
		return data[index];
	}

	// * SAFETY: index must be less than size
	constexpr T& operator[](Size index)
	{
		return data[index];
	}
};
