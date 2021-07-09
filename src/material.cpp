#include "hittable.h"

bool Lambertian::scatter(const Ray &ray, const HitRecord &record, Color &emitted, Color &attenuation, Ray &scattered) const {
	emitted.zero();
	scattered.origin = ray.at(record.t);
	const Vec3 normal = record.hittable->getNormal(scattered.origin, ray);
	attenuation = albedo->value(record.hittable, scattered.origin, normal);
	Scalar n = Random::real();
	const Scalar phi = Random::angle();
	if(std::abs(normal.x) < std::abs(normal.y) && std::abs(normal.x) < std::abs(normal.z)) {
		const Scalar nyz = normal.y*normal.y + normal.z*normal.z;
		const Scalar o = std::sqrt(n / nyz);
		const Scalar co = o * std::cos(phi), si = o * std::sin(phi);
		n = std::sqrt(1. - n);
		scattered.direction.x = n * normal.x + si * nyz;
		scattered.direction.y = n * normal.y + co * normal.z - si * normal.y * normal.x;
		scattered.direction.z = n * normal.z - co * normal.y - si * normal.z * normal.x;
	} else if(std::abs(normal.y) < std::abs(normal.z)) {
		const Scalar nzx = normal.z*normal.z + normal.x*normal.x;
		const Scalar o = std::sqrt(n / nzx);
		const Scalar co = o * std::cos(phi), si = o * std::sin(phi);
		n = std::sqrt(1. - n);
		scattered.direction.x = n * normal.x - co * normal.z - si * normal.x * normal.y;
		scattered.direction.y = n * normal.y + si * nzx;
		scattered.direction.z = n * normal.z + co * normal.x - si * normal.z * normal.y;
	} else {
		const Scalar nxy = normal.x*normal.x + normal.y*normal.y;
		const Scalar o = std::sqrt(n / nxy);
		const Scalar co = o * std::cos(phi), si = o * std::sin(phi);
		n = std::sqrt(1. - n);
		scattered.direction.x = n * normal.x + co * normal.y - si * normal.x * normal.z;
		scattered.direction.y = n * normal.y - co * normal.x - si * normal.y * normal.z;
		scattered.direction.z = n * normal.z + si * nxy;
	}
	return true;
}

bool Metal::scatter(const Ray &ray, const HitRecord &record, Color &emitted, Color &attenuation, Ray &scattered) const {
	emitted.zero();
	attenuation = albedo;
	scattered.origin = ray.at(record.t);
	const Vec3 normal = record.hittable->getNormal(scattered.origin, ray);
	scattered.direction = (reflect(ray.direction, normal) + fuzz * Vec3::randomBall()).normalized();
	return dot(scattered.direction, normal) > 0.;
}

bool Dielectric::scatter(const Ray &ray, const HitRecord &record, Color &emitted, Color &attenuation, Ray &scattered) const {
	emitted.zero();
	attenuation = Color(1., 1., 1.);
	scattered.origin = ray.at(record.t);
	const Vec3 normal = record.hittable->getNormal(scattered.origin, ray);
	const Scalar cosTheta = - dot(ray.direction, normal);
	const Scalar sinTheta = std::sqrt(1. - cosTheta * cosTheta);
   	const Scalar n1_n2 = cosTheta > 0. ? 1. / reflectiveIndex : reflectiveIndex;
    if(n1_n2 * sinTheta > 1.) {
        scattered.direction = ray.direction + 2. * cosTheta * normal;
    } else {
		Scalar proba = (n1_n2 - 1.) / (n1_n2 + 1.);
		proba *= proba;
		proba += (1. - proba) * std::pow(1. - std::abs(cosTheta), 5.);
		if(Random::real() < proba) scattered.direction= ray.direction + 2. * cosTheta * normal;
		else {
			const Vec3 newDir = n1_n2 * (ray.direction + cosTheta * normal);
			if(cosTheta > 0.) scattered.direction = newDir - std::sqrt(std::max(0., 1. - newDir.norm2())) * normal;
			else scattered.direction = newDir + std::sqrt(std::max(0., 1. - newDir.norm2())) * normal;
		}
	}
	return true;
}

bool DiffuseLight::scatter(const Ray &ray, const HitRecord &record, Color &emitted, Color &, Ray &scattered) const {
	scattered.origin = ray.at(record.t);
	emitted = emit->value(record.hittable, scattered.origin, record.hittable->getNormal(scattered.origin, ray));
	return false;
}