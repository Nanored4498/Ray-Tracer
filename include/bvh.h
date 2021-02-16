#pragma once

#include "hittable.h"

class BVHNode : public Hittable {
public:
	BVHNode(HittableList &list): BVHNode(list.begin(), list.size()) {}
	BVHNode(std::vector<std::shared_ptr<const Hittable>>::iterator start, size_t nb);

	bool hit(const Ray &ray, Scalar tMax, HitRecord &record) const override;

	static unsigned long long getNbIntersections() { return nbIntersections; }

private:
	inline static unsigned long long nbIntersections = 0;

	std::shared_ptr<const Hittable> left, right;
};