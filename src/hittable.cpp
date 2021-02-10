#include "hittable.h"

bool HittableList::hit(const Ray &ray, double tMax, HitRecord &record) const {
	double any_hit = false;
	for(Hittable *object : objects) {
		if(object->hit(ray, tMax, record)) {
			tMax = record.t;
			any_hit = true;
		}	
	}
	return any_hit;
}