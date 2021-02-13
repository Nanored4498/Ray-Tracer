#pragma once

#include "hittable.h"

// #define BVH_STATS

class BVHNode : public Hittable {
public:
	BVHNode(HittableList &list): BVHNode(list.begin(), list.end()) {}
	BVHNode(std::vector<std::shared_ptr<Hittable>>::iterator start, std::vector<std::shared_ptr<Hittable>>::iterator end);

	bool hit(const Ray &ray, double tMax, HitRecord &record) const override;

	bool boundingBox(AABB &aabb) const override {
		aabb = box;
		return true;
	}

	static unsigned long long getNbIntersections() { return nbIntersections; }

private:
	inline static unsigned long long nbIntersections = 0;

	std::shared_ptr<Hittable> left, right;
	AABB box;
};