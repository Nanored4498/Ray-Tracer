#pragma once

#include "vec.h"

class PDF {
public:
	virtual ~PDF() {}

	virtual Scalar value(const Vec3 &normal, const Ray &ray) const = 0;
	virtual Scalar generate(const Vec3 &normal, Ray &ray) const = 0;
};

class UniformPDF : public PDF {
public:
	UniformPDF() {}

	inline Scalar value(UNUSUED const Vec3 &normal, UNUSUED const Ray &ray) const override {
		return pdf_val;
	}

	inline Scalar generate(UNUSUED const Vec3 &normal, Ray &ray) const override {
		ray.direction = Vec3::randomSphere();
		return pdf_val;
	}

	static const PDF *instance;
	static constexpr Scalar pdf_val = 1. / (4. * M_PI);
};

class CosinePDF : public PDF {
public:
	CosinePDF(Scalar power): power(power) {}

	inline Scalar value(const Vec3 &normal, const Ray &ray) const override {
		const Scalar cosTheta = dot(normal, ray.direction);
		return cosTheta <= 0. ? 0. : std::pow(cosTheta, power) * (power + 1.) * (.5 * (1. / M_PI));
	}

	Scalar generate(const Vec3 &normal, Ray &ray) const override;

private:
	const Scalar power;
};

class ConePDF : public PDF {
public:
	ConePDF(Scalar cosMax): cosMax(cosMax), val(1. / (2. * M_PI * (1. - cosMax))) {}

	inline Scalar value(const Vec3 &normal, const Ray &ray) const override {
		const Scalar cosTheta = dot(normal, ray.direction);
		return cosTheta < cosMax ? 0. : val;
	}

	Scalar generate(const Vec3 &normal, Ray &ray) const override;

private:
	const Scalar cosMax;
	const Scalar val;
};

class TargetCosinePDF : public PDF {
public:
	TargetCosinePDF(const Vec3 &pos, const Scalar radius): pos(pos), powerMul(2. / (radius * radius)) {}

	Scalar value(const Vec3 &normal, const Ray &ray) const override;
	Scalar generate(const Vec3 &normal, Ray &ray) const override;

private:
	const Vec3 pos;
	const Scalar powerMul;
};

class TargetConePDF : public PDF {
public:
	TargetConePDF(const Vec3 &pos, const Scalar radius): pos(pos), rad2(radius * radius) {}

	Scalar value(const Vec3 &normal, const Ray &ray) const override;
	Scalar generate(const Vec3 &normal, Ray &ray) const override;

private:
	const Vec3 pos;
	const Scalar rad2;
};
