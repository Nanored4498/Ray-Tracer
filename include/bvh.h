#pragma once

#include "hittable.h"

class BVHNode : public Hittable {
public:
	BVHNode(HittableList &list): BVHNode(list.begin(), list.end()) {}
	BVHNode(std::vector<std::shared_ptr<Hittable>>::iterator start, std::vector<std::shared_ptr<Hittable>>::iterator end);

	bool hit(const Ray &ray, double tMax, HitRecord &record) const override;

	bool boundingBox(AABB &aabb) const override {
		aabb = box;
		return true;
	}

private:
	std::shared_ptr<Hittable> left, right;
	AABB box;
};