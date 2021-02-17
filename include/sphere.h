#pragma once

#include "hittable.h"

class Sphere : public Hittable {
public:
	Sphere(const Vec3 &center, Scalar radius, Material *material, bool inverted=false):
		center(center),
		radius(radius),
		material(material),
		inverted(inverted) {
		Vec3 r(radius, radius, radius);
		box = AABB(center - r, center + r);
	}

	~Sphere() {}
	
	bool hit(const Ray &ray, Scalar tMax, HitRecord &record) const override;

private:
	Vec3 center;
	Scalar radius;
	std::shared_ptr<const Material> material;
	bool inverted;
};