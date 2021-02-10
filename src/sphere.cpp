#include "sphere.h"

bool Sphere::hit(const Ray &ray, double tMax, HitRecord &record) const {
	Vec3 oc = center - ray.origin();
	double ocd = dot(oc, ray.direction());
	double delta = ocd*ocd + radius*radius - oc.norm2();
	if(delta > 0.) {
		delta = std::sqrt(delta);
		double t = ocd - delta;
		if(t < 1.e-5) {
			t = ocd + delta;
			if(t < 1.e-5) return false;
		}
		if(t < tMax) {
			record.pos = ray.at(t);
			record.normal = (inverted ? center - record.pos : record.pos - center) / radius;
			record.material = material;
			record.t = t;
			return true;
		}
	}
	return false;
}