#pragma once

#include "hittable.h"

class BVHNode : public Hittable {
public:
	BVHNode(HittableList &list): BVHNode(list.begin(), list.size()) {}
	BVHNode(std::vector<std::shared_ptr<const Hittable>>::iterator start, size_t nb);

	bool hit(const Ray &ray, Scalar tMax, HitRecord &record) const override;

	inline bool isNode() const override { return true; }

private:
	std::shared_ptr<const Hittable> left, right;
	friend class BVHTree;
};

class BVHTree : public Hittable {
public:
	BVHTree(HittableList &list): root(new BVHNode(list)) { box = root->boundingBox(); }

	bool hit(const Ray &ray, Scalar tMax, HitRecord &record) const override;

private:
	std::shared_ptr<const BVHNode> root;
};