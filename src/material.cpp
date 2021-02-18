#include "hittable.h"

bool Lambertian::scatter(const Ray &ray, const HitRecord &record, Color &attenuation, Ray &scattered) const {
	Vec3 pos = ray.at(record.t), normal = record.hittable->getNormal(pos, ray);
	Vec2 uv = record.hittable->getUV(pos, normal);
	attenuation = albedo->value(uv.x, uv.y, pos);
	scattered = Ray(pos, (normal + Vec3::randomBall()).normalized());
	return true;
}

bool Metal::scatter(const Ray &ray, const HitRecord &record, Color &attenuation, Ray &scattered) const {
	Vec3 pos = ray.at(record.t), normal = record.hittable->getNormal(pos, ray);
	attenuation = albedo;
	scattered = Ray(pos, (reflect(ray.direction(), normal) + fuzz * Vec3::randomBall()).normalized());
	return dot(scattered.direction(), normal) > 0.;
}

bool Dielectric::scatter(const Ray &ray, const HitRecord &record, Color &attenuation, Ray &scattered) const {
	Vec3 pos = ray.at(record.t), normal = record.hittable->getNormal(pos, ray);
	attenuation = Color(1., 1., 1.);
	Scalar cosTheta = - dot(ray.direction(), normal);
	Scalar sinTheta = std::sqrt(1. - cosTheta * cosTheta);
   	Scalar n1_n2 = cosTheta > 0. ? 1. / reflectiveIndex : reflectiveIndex;
    if(n1_n2 * sinTheta > 1.) {
        scattered = Ray(pos, ray.direction() + 2. * cosTheta * normal);
    } else {
		Scalar proba = (n1_n2 - 1.) / (n1_n2 + 1.);
		proba *= proba;
		proba += (1. - proba) * std::pow(1. - std::abs(cosTheta), 5.);
		if(Random::real() < proba) scattered = Ray(pos, ray.direction() + 2. * cosTheta * normal);
		else {
			Vec3 newDir = n1_n2 * (ray.direction() + cosTheta * normal);
			if(cosTheta > 0.) scattered = Ray(pos, newDir - std::sqrt(std::max(0., 1. - newDir.norm2())) * normal);
			else scattered = Ray(pos, newDir + std::sqrt(std::max(0., 1. - newDir.norm2())) * normal);
		}
	}
	return true;
}