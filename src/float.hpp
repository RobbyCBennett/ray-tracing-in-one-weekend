#pragma once


using Float = float;
// using Float = double;

extern "C" float powf(float base, float exponent);
extern "C" double pow(double base, double exponent);

extern "C" float sqrtf(float arg);
extern "C" double sqrt(double arg);

extern "C" float tanf(float arg);
extern "C" double tan(double arg);


constexpr Float INFINITY = 1e+308 * 10;

constexpr Float PI = 3.14159265358979323846264338327950288;


constexpr Float degrees_to_radians(Float degrees)
{
	constexpr Float MULTIPLIER = PI / 180;
	return degrees * MULTIPLIER;
}


inline float power(float base, float exponent)
{
	return powf(base, exponent);
}


inline double power(double base, double exponent)
{
	return pow(base, exponent);
}


inline float square_root(float value)
{
	return sqrtf(value);
}


inline double square_root(double value)
{
	return sqrt(value);
}


inline float tangent(float value)
{
	return tanf(value);
}


inline double tangent(double value)
{
	return tan(value);
}


inline Float linear_to_gamma(Float linear_component)
{
	return (linear_component > 0) ? square_root(linear_component) : 0;
}
