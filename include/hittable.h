#pragma once

#include "material.h"
#include "aabb.h"
#include <vector>
#include <memory>

struct HitRecord {
	Vec3 pos, normal;
	const Material *material;
	Scalar t;
};

class Hittable {
public:
	virtual ~Hittable() {}

	virtual bool hit(const Ray &ray, Scalar tMax, HitRecord &record) const = 0;
	
	bool hitBox(const Ray &ray, Scalar tMax, Scalar &t) const { return box.hit(ray, tMax, t); }
	const AABB& boundingBox() const { return box; }

protected:
	AABB box;
};

class HittableList : public Hittable {
public:
	~HittableList() {}

	bool hit(const Ray &ray, Scalar tMax, HitRecord &record) const override;

	void add(const Hittable *object);

	std::vector<std::shared_ptr<const Hittable>>::iterator begin() { return objects.begin(); }
	std::vector<std::shared_ptr<const Hittable>>::iterator end() { return objects.end(); }

private:
	std::vector<std::shared_ptr<const Hittable>> objects;
};