#pragma once

#include "hittable.h"

class ConstantMedium : public Hittable {
public:
	ConstantMedium(std::shared_ptr<Hittable> b, Scalar density, const Color color):
		boundary(std::move(b)),
		negInvDensity(-1./density),
		color(color) {
		box = boundary->boundingBox();
	}
	
	bool hit(const Ray &ray, Scalar tMax, HitRecord &record) const override;
	bool scatter(const Ray &ray, const HitRecord &record, ScatterRecord &out) const override;
	inline virtual Scalar scattering_pdf(UNUSUED const Vec3 &normal, UNUSUED const Vec3 &scattered) const override {
		return UniformPDF::instance->value(normal, scattered);
	}

	inline Vec3 getNormal(const Vec3 &, const Ray &) const override { return Vec3::randomSphere(); }

private:
	static const PDF *pdf;
	std::shared_ptr<const Hittable> boundary;
	Scalar negInvDensity;
	Color color;
};