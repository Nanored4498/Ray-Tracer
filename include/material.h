#pragma once

#include "vec.h"

struct HitRecord;

class Material {
public:
	virtual ~Material() {}

	virtual bool scatter(const Ray &ray, const HitRecord &record, Color &attenuation, Ray &scattered) const = 0;
};

class Lambertian : public Material {
public:
	Lambertian(const Color &albedo): albedo(albedo) {}
	~Lambertian() {}

	bool scatter(const Ray &ray, const HitRecord &record, Color &attenuation, Ray &scattered) const override;

private:
	Color albedo;
};

class Metal : public Material {
public:
	Metal(const Color &albedo, double fuzz): albedo(albedo), fuzz(fuzz) {}
	~Metal() {}

	bool scatter(const Ray &ray, const HitRecord &record, Color &attenuation, Ray &scattered) const override;

private:
	Color albedo;
	double fuzz;
};

class Dielectric : public Material {
public:
	Dielectric(double reflectiveIndex): reflectiveIndex(reflectiveIndex) {}
	~Dielectric() {}

	bool scatter(const Ray &ray, const HitRecord &record, Color &attenuation, Ray &scattered) const override;

private:
	double reflectiveIndex;
};

