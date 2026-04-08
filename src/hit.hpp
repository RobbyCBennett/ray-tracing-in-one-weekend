#pragma once


#include "material.hpp"
#include "ray.hpp"


// A collision of a ray and an hittable object
struct Hit
{
	Vec3 point;
	Vec3 normal;
	Material material;
	Float times;
	bool front_face;

	// * ENSURE: `-1 >= outward_normal >= 1`
	constexpr void set_face_normal(const Ray& ray, const Vec3& outward_normal)
	{
		front_face = dot(ray.direction, outward_normal) < 0;
		normal = front_face ? outward_normal : -outward_normal;
	}
};


inline bool scatter(const Material& material, const Ray& in, const Hit& hit, Vec3& attenuation, Ray& scattered)
{
	using Kind = MaterialKind;

	switch (material.kind) {
		case Kind::Dielectric: {
			const Dielectric& dielectric = material.dielectric;

			attenuation = Color(1, 1, 1);
			Float ri = hit.front_face ? (1 / dielectric.refraction_index) : dielectric.refraction_index;

			Vec3 unit_direction = in.direction.unit();
			Float cos_theta = minimum(dot(-unit_direction, hit.normal), (Float) 1);
			Float sin_theta = square_root(1 - cos_theta * cos_theta);

			bool cannot_refract = ri * sin_theta > 1;
			Vec3 direction;
			if (cannot_refract or Dielectric::reflectance(cos_theta, ri) > random())
				direction = unit_direction.reflect(hit.normal);
			else
				direction = unit_direction.refract(hit.normal, ri);

			scattered = Ray(hit.point, direction);
			return true;
		}
		case Kind::Lambertian: {
			const Lambertian& lambertian = material.lambertian;

			Vec3 scatter_direction = hit.normal + Vec3::random_unit();

			// Catch degenerate scatter direction
			if (scatter_direction.near_zero())
				scatter_direction = hit.normal;

			scattered = Ray(hit.point, scatter_direction);
			attenuation = lambertian.albedo;
			return true;
		}
		case Kind::Metal: {
			const Metal& metal = material.metal;

			Vec3 reflected = in.direction.reflect(hit.normal);
			reflected = reflected.unit() + (metal.fuzz * Vec3::random_unit());
			scattered = Ray(hit.point, reflected);
			attenuation = metal.albedo;
			return dot(scattered.direction, hit.normal) > 0;
		}
		default:
			return false;
	}
}
