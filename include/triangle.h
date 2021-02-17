#pragma once

#include "hittable.h"

class Triangle : public Hittable {
public:
	Triangle(const Vec3 &a, const Vec3 &b, const Vec3 &c, std::shared_ptr<const Material> material, bool biface=false);

	~Triangle() {}
	
	bool hit(const Ray &ray, Scalar tMax, HitRecord &record) const override;

	inline bool scatter(const Ray &ray, const HitRecord &record, Color &attenuation, Ray &scattered) const override {
		return material->scatter(ray, record, attenuation, scattered);
	}

	inline Vec3 getNormal(const Vec3 &, const Ray &ray) const override {
		return biface && dot(normal, ray.direction()) > 0. ? -normal : normal;
	}

	inline Vec2 getUV(const Vec3 &pos, const Vec3 &) const override {
		Scalar x = pos[(1+fixedColumn)%3], y = pos[(2+fixedColumn)%3];
		return Vec2(invT[0] * x + invT[1] * y + invT[2],
					invT[3] * x + invT[4] * y + invT[5]);
	}

private:
	Vec3 normal;
	std::shared_ptr<const Material> material;
	bool biface;
	unsigned char fixedColumn;
	Scalar invT[9];
};

void loadOBJ(const std::string &fileName, HittableList &list, const Vec3 &rotAxis, Scalar angle, Scalar scale, const Vec3 &pos);