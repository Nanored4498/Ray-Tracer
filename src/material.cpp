#include "hittable.h"

bool Lambertian::scatter(const Ray &, const HitRecord &record, Color &attenuation, Ray &scattered) const {
	attenuation = albedo;
	scattered = Ray(record.pos, (record.normal + Vec3::randomBall()).normalized());
	return true;
}

bool Metal::scatter(const Ray &ray, const HitRecord &record, Color &attenuation, Ray &scattered) const {
	attenuation = albedo;
	scattered = Ray(record.pos, (reflect(ray.direction(), record.normal) + fuzz * Vec3::randomBall()).normalized());
	return dot(scattered.direction(), record.normal) > 0.;
}

bool Dielectric::scatter(const Ray &ray, const HitRecord &record, Color &attenuation, Ray &scattered) const {
	attenuation = Color(1., 1., 1.);
	Scalar cosTheta = - dot(ray.direction(), record.normal);
	Scalar sinTheta = std::sqrt(1. - cosTheta * cosTheta);
   	Scalar n1_n2 = cosTheta > 0. ? 1. / reflectiveIndex : reflectiveIndex;
    if(n1_n2 * sinTheta > 1.) {
        scattered = Ray(record.pos, ray.direction() + 2. * cosTheta * record.normal);
    } else {
		Scalar proba = (n1_n2 - 1.) / (n1_n2 + 1.);
		proba *= proba;
		proba += (1. - proba) * std::pow(1. - std::abs(cosTheta), 5.);
		if(Random::real() < proba) scattered = Ray(record.pos, ray.direction() + 2. * cosTheta * record.normal);
		else {
			Vec3 newDir = n1_n2 * (ray.direction() + cosTheta * record.normal);
			if(cosTheta > 0.) scattered = Ray(record.pos, newDir - std::sqrt(std::max(0., 1. - newDir.norm2())) * record.normal);
			else scattered = Ray(record.pos, newDir + std::sqrt(std::max(0., 1. - newDir.norm2())) * record.normal);
		}
	}
	return true;
}