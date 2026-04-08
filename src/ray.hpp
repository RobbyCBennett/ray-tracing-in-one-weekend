#pragma once


#include "vec3.hpp"


// Two points in 3D space
struct Ray
{
	Vec3 origin;
	Vec3 direction;

	Ray() = default;

	constexpr Ray(Vec3 origin_, Vec3 direction_):
		origin(origin_), direction(direction_)
	{}

	constexpr Vec3 at(Float t) const
	{
		return origin + t * direction;
	}
};
