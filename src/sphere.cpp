#include "sphere.h"

#include "stats.h"

std::atomic<unsigned long long> Stats::sphereRayTest = {0uLL};
thread_local unsigned long long Stats::localSphereRayTest = 0uLL;

bool Sphere::hit(const Ray &ray, Scalar tMax, HitRecord &record) const {
	UPDATE_SPHERE_STATS
	Vec3 oc = center - ray.origin();
	Scalar ocd = dot(oc, ray.direction());
	Scalar delta = ocd*ocd + radius*radius - oc.norm2();
	if(delta > 0.) {
		delta = std::sqrt(delta);
		Scalar t = ocd - delta;
		if(t <= EPS) {
			t = ocd + delta;
			if(t <= EPS) return false;
		}
		if(t < tMax) {
			record.pos = ray.at(t);
			record.normal = (inverted ? center - record.pos : record.pos - center) / radius;
			record.material = material.get();
			record.t = t;
			return true;
		}
	}
	return false;
}