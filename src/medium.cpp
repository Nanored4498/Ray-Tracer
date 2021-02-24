#include "medium.h"

#include <iostream>

// Suppose boundary to be convex and bounded
bool ConstantMedium::hit(const Ray &ray, Scalar tMax, HitRecord &record) const {
	HitRecord rec;
	const Scalar reverseDist = 2.*EPS + std::max(std::abs(ray.origin.x - box.min().x), std::abs(ray.origin.x - box.max().x))
								+ std::max(std::abs(ray.origin.y - box.min().y), std::abs(ray.origin.y - box.max().y))
								+ std::max(std::abs(ray.origin.z - box.min().z), std::abs(ray.origin.z - box.max().z));
	Ray newRay(ray.origin - reverseDist*ray.direction, ray.direction);
	if(tMax != std::numeric_limits<Scalar>::max()) tMax += reverseDist;
	if(!boundary->hit(newRay, tMax, rec)) return false;
	Scalar t = std::max(reverseDist, rec.t) + negInvDensity * std::log(Random::real());
	if(t > tMax) return false;
	rec.t += 2.*EPS;
	newRay.origin = newRay.at(rec.t);
	if(boundary->hit(newRay, t - rec.t, rec)) return false;
	record.t = t - reverseDist;
	record.hittable = this;
	return true;
}

bool ConstantMedium::scatter(const Ray &ray, const HitRecord &record, Color &emitted, Color &attenuation, Ray &scattered) const {
	emitted.zero();
	attenuation = color;
	scattered.origin = ray.at(record.t);
	scattered.direction = Vec3::randomSphere();
	return true;
}