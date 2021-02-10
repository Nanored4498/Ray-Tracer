#pragma once

#include "material.h"
#include <vector>

struct HitRecord {
	Vec3 pos, normal;
	const Material *material;
	double t;
};

class Hittable {
public:
	virtual ~Hittable() {}

	virtual bool hit(const Ray &ray, double tMax, HitRecord &record) const = 0;
};

class HittableList : public Hittable {
public:
	~HittableList() {
		for(Hittable *object : objects) delete object;
	}

	bool hit(const Ray &ray, double tMax, HitRecord &record) const override;

	void add(Hittable *object) { objects.push_back(object); }

private:
	std::vector<Hittable*> objects;
};