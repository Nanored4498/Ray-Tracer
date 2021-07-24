#include "hittable.h"

const PDF* Lambertian::pdf = new CosinePDF(1.);

bool Lambertian::scatter(UNUSUED const Ray &ray, const HitRecord &record, ScatterRecord &out) const {
	out.emitted.zero();
	out.attenuation = albedo->value(record.hittable, out.ray.origin, record.normal);
	out.isSpecular = false;
	out.pdf = pdf;
	return true;
}

bool Metal::scatter(const Ray &ray, const HitRecord &record, ScatterRecord &out) const {
	out.emitted.zero();
	out.attenuation = albedo;
	out.isSpecular = true;
	out.ray.direction = (reflect(ray.direction, record.normal) + fuzz * Vec3::randomBall()).normalized();
	return dot(out.ray.direction, record.normal) > 0.;
}

bool Dielectric::scatter(const Ray &ray, const HitRecord &record, ScatterRecord &out) const {
	out.emitted.zero();
	out.attenuation = Color(1., 1., 1.);
	out.isSpecular = true;
	const Vec3 &normal = record.normal;
	const Scalar cosTheta = - dot(ray.direction, normal);
	const Scalar sinTheta = std::sqrt(1. - cosTheta * cosTheta);
   	const Scalar n1_n2 = cosTheta > 0. ? 1. / reflectiveIndex : reflectiveIndex;
    if(n1_n2 * sinTheta > 1.) { // fully reflected
        out.ray.direction = ray.direction + 2. * cosTheta * normal;
    } else { // probabilistic
		Scalar proba = (n1_n2 - 1.) / (n1_n2 + 1.);
		proba *= proba;
		proba += (1. - proba) * std::pow(1. - std::abs(cosTheta), 5.);
		if(Random::real() < proba) out.ray.direction = ray.direction + 2. * cosTheta * normal;
		else { // Refraction
			out.ray.direction = n1_n2 * (ray.direction + cosTheta * normal);
			const Scalar opp_norm2 = 1. - out.ray.direction.norm2();
			if(opp_norm2 > 0.) {
				if(cosTheta > 0.) out.ray.direction -= std::sqrt(opp_norm2) * normal;
				else out.ray.direction += std::sqrt(opp_norm2) * normal;
			}
		}
	}
	return true;
}

bool DiffuseLight::scatter(const Ray &ray, const HitRecord &record, ScatterRecord &out) const {
	if(dot(record.normal, ray.direction) < 0.) out.emitted = emit->value(record.hittable, out.ray.origin, record.normal);
	else out.emitted.zero();
	return false;
}