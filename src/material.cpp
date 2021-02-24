#include "hittable.h"

bool Lambertian::scatter(const Ray &ray, const HitRecord &record, Color &emitted, Color &attenuation, Ray &scattered) const {
	emitted.zero();
	scattered.origin = ray.at(record.t);
	Vec3 normal = record.hittable->getNormal(scattered.origin, ray);
	Vec2 uv = record.hittable->getUV(scattered.origin, normal);
	attenuation = albedo->value(uv.x, uv.y, scattered.origin);
	scattered.direction = Vec3::randomSphere();
	scattered.direction -= dot(scattered.direction, normal) * normal;
	Scalar z = Random::real();
	scattered.direction *= std::sqrt(1. - z) / scattered.direction.norm();
	scattered.direction += sqrt(z) * normal;
	return true;
}

bool Metal::scatter(const Ray &ray, const HitRecord &record, Color &emitted, Color &attenuation, Ray &scattered) const {
	emitted.zero();
	attenuation = albedo;
	scattered.origin = ray.at(record.t);
	Vec3 normal = record.hittable->getNormal(scattered.origin, ray);
	scattered.direction = (reflect(ray.direction, normal) + fuzz * Vec3::randomBall()).normalized();
	return dot(scattered.direction, normal) > 0.;
}

bool Dielectric::scatter(const Ray &ray, const HitRecord &record, Color &emitted, Color &attenuation, Ray &scattered) const {
	emitted.zero();
	attenuation = Color(1., 1., 1.);
	scattered.origin = ray.at(record.t);
	Vec3 normal = record.hittable->getNormal(scattered.origin, ray);
	Scalar cosTheta = - dot(ray.direction, normal);
	Scalar sinTheta = std::sqrt(1. - cosTheta * cosTheta);
   	Scalar n1_n2 = cosTheta > 0. ? 1. / reflectiveIndex : reflectiveIndex;
    if(n1_n2 * sinTheta > 1.) {
        scattered.direction = ray.direction + 2. * cosTheta * normal;
    } else {
		Scalar proba = (n1_n2 - 1.) / (n1_n2 + 1.);
		proba *= proba;
		proba += (1. - proba) * std::pow(1. - std::abs(cosTheta), 5.);
		if(Random::real() < proba) scattered.direction= ray.direction + 2. * cosTheta * normal;
		else {
			Vec3 newDir = n1_n2 * (ray.direction + cosTheta * normal);
			if(cosTheta > 0.) scattered.direction = newDir - std::sqrt(std::max(0., 1. - newDir.norm2())) * normal;
			else scattered.direction = newDir + std::sqrt(std::max(0., 1. - newDir.norm2())) * normal;
		}
	}
	return true;
}

bool DiffuseLight::scatter(const Ray &ray, const HitRecord &record, Color &emitted, Color &, Ray &) const {
	Vec3 pos = ray.at(record.t), normal = record.hittable->getNormal(pos, ray);
	Vec2 uv = record.hittable->getUV(pos, normal);
	emitted = emit->value(uv.x, uv.y, pos);
	return false;
}