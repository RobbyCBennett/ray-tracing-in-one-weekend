#pragma once


#include "vec3.hpp"


enum class MaterialKind
{
	Dielectric,
	Lambertian,
	Metal,
};


// A refractive material
struct Dielectric
{
	Float refraction_index;

	Dielectric() = default;

	constexpr Dielectric(Float refraction_index_):
		refraction_index(refraction_index_)
	{}

	static Float reflectance(Float cosine, Float refraction_index)
	{
		// Use Schlick's approximation for reflectance
		Float r0 = (1 - refraction_index) / (1 + refraction_index);
		r0 = r0 * r0;
		return r0 + (1 - r0) * power((1 - cosine), 5);
	}
};


// A matte material
struct Lambertian
{
	Color albedo;

	Lambertian() = default;

	constexpr Lambertian(Color albedo_): albedo(albedo_)
	{}
};


// A reflective material
struct Metal
{
	Color albedo;
	Float fuzz;

	Metal() = default;

	constexpr Metal(Color albedo_, Float fuzz_):
		albedo(albedo_), fuzz(fuzz_ < 1 ? fuzz_ : 1)
	{}
};


// Tagged union of material variants
struct Material
{
	using Kind = MaterialKind;

	Kind kind;
	union {
		Dielectric dielectric;
		Lambertian lambertian;
		Metal metal;
	};

	Material() = default;

	constexpr Material(Dielectric dielectric_):
		kind(Kind::Dielectric), dielectric(dielectric_)
	{}

	constexpr Material(Lambertian lambertian_):
		kind(Kind::Lambertian), lambertian(lambertian_)
	{}

	constexpr Material(Metal metal_):
		kind(Kind::Metal), metal(metal_)
	{}

	// A matte gray material
	constexpr static Material plain()
	{
		return Lambertian(Color(0.5, 0.5, 0.5));
	}
};
