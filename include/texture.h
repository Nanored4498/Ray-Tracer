#pragma once

#include "vec.h"

class Texture {
public:
	virtual Color value(Scalar u, Scalar v, const Vec3 &p) const = 0;
};

class SolidColor : public Texture {
public:
	SolidColor(Scalar r, Scalar g, Scalar b): color(r, g, b) {}
	SolidColor(const Color &color): color(color) {}

	inline Color value(Scalar, Scalar, const Vec3 &) const override { return color; }

private:
	Color color;
};

class CheckerTexture : public Texture {
public:
	CheckerTexture() {}
	CheckerTexture(const Color &even, const Color &odd): even(even), odd(odd) {}

	inline Color value(Scalar, Scalar, const Vec3& p) const override {
		if((std::sin(8.*p.x()) < 0.) ^ (std::sin(8.*p.y()) < 0.) ^ (std::sin(8.*p.z()) < 0.)) return odd;
		else return even;
	}
public:
	Color even, odd;
};