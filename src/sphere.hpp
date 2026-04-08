#pragma once


#include "hit.hpp"
#include "interval.hpp"
#include "material.hpp"
#include "ray.hpp"


struct Sphere
{
	Vec3 center;
	Float radius;
	Material material;

	Sphere() = default;

	constexpr Sphere(Vec3 center_, Float radius_, Material material_):
		center(center_), radius((radius_ > 0) ? radius_ : 0), material(material_)
	{}

	bool hit(const Ray& ray, Interval ray_times, Hit& hit) const
	{
		Vec3 d_origins = center - ray.origin;

		// Simplification of the quadratic equation
		Float a = ray.direction.length_squared();
		Float h = dot(ray.direction, d_origins);
		Float c = d_origins.length_squared() - radius * radius;
		Float discriminant = h * h - a * c;
		if (discriminant < 0)
			return false;

		Float discriminant_root = square_root(discriminant);

		// Find the nearest root that lies in the acceptable range
		Float root = (h - discriminant_root) / a;
		if (root <= ray_times.min or ray_times.max <= root) {
			root = (h + discriminant_root) / a;
			if (root <= ray_times.min or ray_times.max <= root)
				return false;
		}

		hit.times = root;
		hit.point = ray.at(hit.times);
		Vec3 outward_normal = (hit.point - center) / radius;
		hit.set_face_normal(ray, outward_normal);
		hit.material = material;

		return true;
	}
};
