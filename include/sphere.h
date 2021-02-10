#pragma once

#include "hittable.h"

class Sphere : public Hittable {
public:
	Sphere(const Vec3 &center, double radius, Material *material, bool inverted=false):
		center(center), radius(radius), material(material), inverted(inverted) {}

	~Sphere() { delete material; }
	
	bool hit(const Ray &ray, double tMax, HitRecord &record) const override;
	bool boundingBox(AABB& aabb) const override;

private:
	Vec3 center;
	double radius;
	Material *material;
	bool inverted;
};