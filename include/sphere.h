#pragma once

#include "hittable.h"

class Sphere : public Hittable {
public:
	Sphere(const Vec3 &center, Scalar radius, std::shared_ptr<Material> material, bool inverted=false):
		center(center),
		radius(radius),
		material(std::move(material)),
		inverted(inverted) {
		Vec3 r(radius, radius, radius);
		box = AABB(center - r, center + r);
	}
	
	bool hit(const Ray &ray, Scalar tMax, HitRecord &record) const override;

	inline bool scatter(const Ray &ray, const HitRecord &record, Color &emitted, Color &attenuation, Ray &scattered) const override {
		return material->scatter(ray, record, emitted, attenuation, scattered);
	}
	inline virtual Scalar scattering_pdf(const Ray &scattered, const Vec3 &normal) const override {
		return material->scattering_pdf(scattered, normal);
	}

	inline Vec3 getNormal(const Vec3 &pos, const Ray &) const override {
		if(inverted) return (center - pos) / radius;
		else return (pos - center) / radius;
	}

	inline Vec2 getUV(const Vec3 &, const Vec3 &normal) const override {
		return Vec2(.5 + std::atan2(-normal.z, normal.x) / (2.*M_PI), std::acos(-normal.y) / M_PI);
	}

private:
	Vec3 center;
	Scalar radius;
	std::shared_ptr<const Material> material;
	bool inverted;
};