#pragma once

#include "vec.h"

class Hittable;

class Texture {
public:
	virtual Color value(const Hittable *hittable, const Vec3 &p, const Vec3 &normal) const = 0;
};

class SolidColor : public Texture {
public:
	SolidColor(Scalar r, Scalar g, Scalar b): color(r, g, b) {}
	SolidColor(const Color &color): color(color) {}

	inline Color value(const Hittable *, const Vec3 &, const Vec3 &) const override { return color; }

private:
	Color color;
};

class CheckerTexture : public Texture {
public:
	CheckerTexture() {}
	CheckerTexture(const Color &even, const Color &odd): even(even), odd(odd) {}

	inline Color value(const Hittable *, const Vec3 &p, const Vec3 &) const override {
		if((std::sin(8.*p.x) < 0.) ^ (std::sin(8.*p.y) < 0.) ^ (std::sin(8.*p.z) < 0.)) return odd;
		else return even;
	}

private:
	Color even, odd;
};

class NoiseTexture : public Texture {
public:
	NoiseTexture(double scale);
	~NoiseTexture() {
		delete[] vecs;
		delete[] perms[0];
		delete[] perms[1];
	}

	Color value(const Hittable *hittable, const Vec3 &p, const Vec3 &normal) const override;

private:
	static const int nbVals = 1<<8;
	Vec3 *vecs;
	int *perms[2];
	double scale;
};

class ImageTexture : public Texture {
public:
	ImageTexture(std::string fileName);
	~ImageTexture() { if(data != nullptr) delete data; }

	Color value(const Hittable *hittable, const Vec3 &p, const Vec3 &normal) const override;

private:
	u_char *data;
	int W, H, C;
};