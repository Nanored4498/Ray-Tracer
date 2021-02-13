#include "hittable.h"

bool HittableList::hit(const Ray &ray, double tMax, HitRecord &record) const {
	double any_hit = false;
	for(const std::shared_ptr<Hittable> &object : objects) {
		if(object->hit(ray, tMax, record)) {
			tMax = record.t;
			any_hit = true;
		}	
	}
	return any_hit;
}

void HittableList::add(Hittable *object) {
	if(objects.empty()) box = object->boundingBox();
	else box.surround(object->boundingBox());
	objects.emplace_back(object);
}