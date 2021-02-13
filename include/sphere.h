#pragma once

#include "hittable.h"

// #define SPHERE_STATS

class Sphere : public Hittable {
public:
	Sphere(const Vec3 &center, double radius, Material *material, bool inverted=false):
		center(center),
		radius(radius),
		material(material),
		inverted(inverted) {
		Vec3 r(radius, radius, radius);
		box = AABB(center - r, center + r);
	}

	~Sphere() { delete material; }
	
	bool hit(const Ray &ray, double tMax, HitRecord &record) const override;

	static unsigned long long getNbIntersections() { return nbIntersections; }

private:
	inline static unsigned long long nbIntersections = 0;

	Vec3 center;
	double radius;
	Material *material;
	bool inverted;
};