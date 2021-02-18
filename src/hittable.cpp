#include "hittable.h"

bool HittableList::hit(const Ray &ray, Scalar tMax, HitRecord &record) const {
	Scalar any_hit = false;
	for(const std::shared_ptr<const Hittable> &object : objects) {
		if(object->hit(ray, tMax, record)) {
			tMax = record.t;
			any_hit = true;
		}	
	}
	return any_hit;
}

void HittableList::add(const Hittable *object) {
	if(objects.empty()) box = object->boundingBox();
	else box.surround(object->boundingBox());
	objects.emplace_back(object);
}

void HittableList::add(std::shared_ptr<Hittable> object) {
	if(objects.empty()) box = object->boundingBox();
	else box.surround(object->boundingBox());
	objects.emplace_back(std::move(object));
}