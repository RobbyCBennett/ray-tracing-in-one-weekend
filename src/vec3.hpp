#pragma once


#include "float.hpp"
#include "int.hpp"
#include "number.hpp"
#include "random.hpp"


struct Vec3;

extern "C" float fabsf(float arg);
extern "C" double fabs(double arg);

constexpr Float dot(const Vec3& vec1, const Vec3& vec2);

// One divided by the square root of a float
inline float inverse_square_root(float scalar);
// One divided by the square root of a double
inline double inverse_square_root(double scalar);

constexpr Vec3 operator+(const Vec3& vec1, const Vec3& vec2);
constexpr Vec3 operator-(const Vec3& vec1, const Vec3& vec2);
constexpr Vec3 operator*(const Vec3& vec1, const Vec3& vec2);
constexpr Vec3 operator*(Float scalar, const Vec3& vec);
constexpr Vec3 operator*(const Vec3& vec, Float scalar);
constexpr Vec3 operator/(const Vec3& vec, Float scalar);


// A vector of 3 doubles
struct Vec3
{
	Float x;
	Float y;
	Float z;

	Vec3() = default;

	constexpr Vec3(Float x_, Float y_, Float z_): x(x_), y(y_), z(z_)
	{}

	// Random vector in the unit sphere
	static Vec3 random_unit()
	{
		return Vec3(random(-1, 1), random(-1, 1), random(-1, 1)).unit();
	}

	// Random vector in the unit disk
	static Vec3 random_in_unit_disk()
	{
		return Vec3(random(-1, 1), random(-1, 1), 0).unit();
	}

	Float length() const
	{
		return square_root(length_squared());
	}

	constexpr Float length_squared() const
	{
		return x * x + y * y + z * z;
	}

	constexpr bool near_zero() const
	{
		constexpr Float EPSILON = 1e-8;
		return
			x < EPSILON and x > -EPSILON and
			y < EPSILON and y > -EPSILON and
			z < EPSILON and z > -EPSILON;
	}

	// For this normal vector, randomly bounce a new vector
	Vec3 random_on_hemisphere() const
	{
		Vec3 unit = random_unit();
		if (unit.x * x + unit.y * y + unit.z * z > 0)
			return unit;
		else
			return -unit;
	}

	Vec3 reflect(const Vec3& normal) const
	{
		return *this - 2 * dot(*this, normal) * normal;
	}

	Vec3 refract(const Vec3& normal, Float etai_over_etat) const
	{
		Float cos_theta = minimum(dot(-*this, normal), (Float) 1);
		Vec3 r_out_perp = etai_over_etat * (*this + cos_theta * normal);
		Vec3 r_out_parallel = -square_root(absolute_value(1 - r_out_perp.length_squared())) * normal;
		return r_out_perp + r_out_parallel;
	}

	// Copy, shrinking the length to fit into the unit sphere
	Vec3 unit() const
	{
		Float scalar = inverse_square_root(length_squared());
		return scalar * *this;
	}

	constexpr Vec3 operator-() const
	{
		return Vec3(-x, -y, -z);
	}

	constexpr Vec3& operator+=(const Vec3& other)
	{
		x += other.x;
		y += other.y;
		z += other.z;
		return *this;
	}

	constexpr Vec3& operator-=(const Vec3& other)
	{
		x -= other.x;
		y -= other.y;
		z -= other.y;
		return *this;
	}

	constexpr Vec3& operator+=(Float scalar)
	{
		x += scalar;
		y += scalar;
		z += scalar;
		return *this;
	}

	constexpr Vec3& operator-=(Float scalar)
	{
		x -= scalar;
		y -= scalar;
		z -= scalar;
		return *this;
	}

	constexpr Vec3& operator*=(Float scalar)
	{
		x *= scalar;
		y *= scalar;
		z *= scalar;
		return *this;
	}

	constexpr Vec3& operator/=(Float scalar)
	{
		return *this *= 1 / scalar;
	}
};


using Color = Vec3;


constexpr Float dot(const Vec3& vec1, const Vec3& vec2)
{
	return vec1.x * vec2.x + vec1.y * vec2.y + vec1.z * vec2.z;
}


constexpr Vec3 cross(const Vec3& vec1, const Vec3& vec2)
{
	Float x = vec1.y * vec2.z - vec1.z * vec2.y;
	Float y = vec1.z * vec2.x - vec1.x * vec2.z;
	Float z = vec1.x * vec2.y - vec1.y * vec2.x;
	return Vec3(x, y, z);
}


inline float inverse_square_root(float scalar)
{
	// Use the x86-64 instruction `rsqrtss`
	return __builtin_ia32_rsqrtss({scalar})[0];
}


inline double inverse_square_root(double scalar)
{
	// Fast inverse square root without Newton method iterations
	S64 i;
	i = *(S64 *) &scalar;              // evil floating point bit level hacking
	i = 0x5fe6eb50c7b537a9 - (i >> 1); // what the fuck?
	scalar = *(double *) &i;
	return scalar;
}


constexpr Vec3 operator+(const Vec3& vec1, const Vec3& vec2)
{
	return Vec3(vec1.x + vec2.x, vec1.y + vec2.y, vec1.z + vec2.z);
}


constexpr Vec3 operator-(const Vec3& vec1, const Vec3& vec2)
{
	return Vec3(vec1.x - vec2.x, vec1.y - vec2.y, vec1.z - vec2.z);
}


constexpr Vec3 operator*(const Vec3& vec1, const Vec3& vec2)
{
	return Vec3(vec1.x * vec2.x, vec1.y * vec2.y, vec1.z * vec2.z);
}


constexpr Vec3 operator*(Float scalar, const Vec3& vec)
{
	return Vec3(scalar * vec.x, scalar * vec.y, scalar * vec.z);
}


constexpr Vec3 operator*(const Vec3& vec, Float scalar)
{
	return scalar * vec;
}


constexpr Vec3 operator/(const Vec3& vec, Float scalar)
{
	return (1 / scalar) * vec;
}
