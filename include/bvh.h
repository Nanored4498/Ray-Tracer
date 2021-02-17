#pragma once

#include "hittable.h"

class BVHNode : public Hittable {
public:
	BVHNode(HittableList &list): BVHNode(list.begin(), list.size()) {}
	BVHNode(std::vector<std::shared_ptr<const Hittable>>::iterator start, size_t nb);

	bool hit(const Ray &ray, Scalar tMax, HitRecord &record) const override;

	inline bool isNode() const override { return true; }

	inline bool scatter(const Ray &, const HitRecord &, Color &, Ray &) const override { return false; }

	inline Vec3 getNormal(const Vec3 &, const Ray &) const override { return Vec3(); }

private:
	std::shared_ptr<const Hittable> left, right;
	friend class BVHTree;
};

class BVHTree : public Hittable {
public:
	BVHTree(HittableList &list): root(new BVHNode(list)) { box = root->boundingBox(); }

	bool hit(const Ray &ray, Scalar tMax, HitRecord &record) const override;

	inline bool scatter(const Ray &, const HitRecord &, Color &, Ray &) const override { return false; }

	inline Vec3 getNormal(const Vec3 &, const Ray &) const override { return Vec3(); }

private:
	std::shared_ptr<const BVHNode> root;
};