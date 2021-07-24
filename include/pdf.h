#pragma once

#include "vec.h"

class PDF {
public:
	virtual ~PDF() {}

	virtual Scalar value(const Vec3 &normal, const Vec3 &direction) const = 0;
	virtual Vec3 generate(const Vec3 &normal) const = 0;
};

class UniformPDF : public PDF {
public:
	UniformPDF() {}

	inline Scalar value(UNUSUED const Vec3 &normal, UNUSUED const Vec3 &direction) const override {
		return 1. / (4. * M_PI);
	}

	inline Vec3 generate(UNUSUED const Vec3 &normal) const override {
		return Vec3::randomSphere();
	}

	static const PDF *instance;
};

class CosinePDF : public PDF {
public:
	CosinePDF(Scalar power): power(power) {}

	inline Scalar value(const Vec3 &normal, const Vec3 &direction) const override {
		const Scalar cosTheta = dot(normal, direction);
		return cosTheta <= 0. ? 0. : std::pow(cosTheta, power) * (power + 1.) * (.5 * (1. / M_PI));
	}

	Vec3 generate(const Vec3 &normal) const override;

private:
	const Scalar power;
};

class ConePDF : public PDF {
public:
	ConePDF(Scalar cosMax): cosMax(cosMax), val(1. / (2. * M_PI * (1. - cosMax))) {}

	inline Scalar value(const Vec3 &normal, const Vec3 &direction) const override {
		const Scalar cosTheta = dot(normal, direction);
		return cosTheta < cosMax ? 0. : val;
	}

	Vec3 generate(const Vec3 &normal) const override;

private:
	const Scalar cosMax;
	const Scalar val;
};
