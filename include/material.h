#pragma once

#include "texture.h"
#include "pdf.h"

#include <memory>

struct HitRecord;

struct ScatterRecord {
	Color emitted;
	Color attenuation;
	bool isSpecular;
	Ray ray;
	const PDF *pdf;
};

class Material {
public:
	virtual ~Material() {}

	virtual bool scatter(const Ray &ray, const HitRecord &record, ScatterRecord &out) const = 0;
	inline virtual Scalar scattering_pdf(UNUSUED const Vec3 &normal, UNUSUED const Vec3 &scattered) const { return 0.; }
};

class Lambertian : public Material {
public:
	Lambertian(const Color &albedo): albedo(std::make_shared<SolidColor>(albedo)) {}
	Lambertian(std::shared_ptr<Texture> albedo): albedo(std::move(albedo)) {}

	bool scatter(const Ray &ray, const HitRecord &record, ScatterRecord &out) const override;
	inline Scalar scattering_pdf(const Vec3 &normal, const Vec3 &scattered) const override {
		return pdf->value(normal, scattered);
	}

private:
	static const PDF *pdf;
	std::shared_ptr<Texture> albedo;
};

class Metal : public Material {
public:
	Metal(const Color &albedo, Scalar fuzz): albedo(albedo), fuzz(fuzz) {}
	~Metal() {}

	bool scatter(const Ray &ray, const HitRecord &record, ScatterRecord &out) const override;

private:
	Color albedo;
	Scalar fuzz;
};

class Dielectric : public Material {
public:
	Dielectric(Scalar reflectiveIndex): reflectiveIndex(reflectiveIndex) {}
	~Dielectric() {}

	bool scatter(const Ray &ray, const HitRecord &record, ScatterRecord &out) const override;

private:
	Scalar reflectiveIndex;
};


class DiffuseLight : public Material {
public:
	DiffuseLight(const Color &emit): emit(std::make_shared<SolidColor>(emit)) {}
	DiffuseLight(Texture *emit): emit(emit) {}
	DiffuseLight(std::shared_ptr<Texture> emit): emit(emit) {}

	bool scatter(const Ray &ray, const HitRecord &record, ScatterRecord &out) const override;

private:
	std::shared_ptr<Texture> emit;
};