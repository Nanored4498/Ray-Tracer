#pragma once

#include "material.h"
#include "aabb.h"
#include <vector>

class Hittable;
struct HitRecord {
	const Hittable *hittable;
	Scalar t;
	Vec3 normal;
};

class Hittable {
public:
	virtual ~Hittable() {}

	virtual bool hit(const Ray &ray, Scalar tMax, HitRecord &record) const = 0;
	
	bool hitBox(const Ray &ray, Scalar tMax, Scalar &t) const { return box.hit(ray, tMax, t); }
	bool hitBoxInv(const Ray &ray, Scalar tMax, Scalar &t) const { return box.hitInv(ray, tMax, t); }
	const AABB& boundingBox() const { return box; }

	inline virtual bool scatter(UNUSUED const Ray &ray, UNUSUED const HitRecord &record, Color &emitted, UNUSUED Color &attenuation, UNUSUED Ray &scattered) const {
		emitted.zero();
		return false;
	}
	inline virtual double scattering_pdf(UNUSUED const Ray &scattered, UNUSUED const Vec3 &normal) const { return 0.; }

	virtual Vec3 getNormal(const Vec3 &pos, const Ray &ray) const = 0;

	inline virtual Vec2 getUV(const Vec3 &, const Vec3 &) const { return Vec2(0., 0.); }

	inline virtual bool isNode() const { return false; }

protected:
	AABB box;
};

class HittableList : public Hittable {
public:
	~HittableList() {}

	bool hit(const Ray &ray, Scalar tMax, HitRecord &record) const override;

	void add(const Hittable *object);
	void add(std::shared_ptr<Hittable> object);

	inline std::vector<std::shared_ptr<const Hittable>>::iterator begin() { return objects.begin(); }
	inline std::vector<std::shared_ptr<const Hittable>>::iterator end() { return objects.end(); }
	inline size_t size() const { return objects.size(); }

	inline Vec3 getNormal(const Vec3 &, const Ray &) const override { return Vec3(); }

private:
	std::vector<std::shared_ptr<const Hittable>> objects;
};