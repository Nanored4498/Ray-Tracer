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

bool HittableList::boundingBox(AABB& aabb) const {
	if(objects.empty()) {
		aabb = AABB();
		return true;
	}
	objects[0]->boundingBox(aabb);
	for(auto obj = objects.begin()+1; obj != objects.end(); ++obj) {
		AABB tmpBox;
		if(!(*obj)->boundingBox(tmpBox)) return false;
		aabb.surround(tmpBox);
	}
	return true;
}