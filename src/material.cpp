#include "hittable.h"

// p(theta) = cos(theta) where theta is the angle with the normal
// sin(theta) is uniform
// scattered = r * t + sqrt(1 - r*r) * n --- (where t and n are tangential and normal vectors and r uniform in [0, 1])
bool Lambertian::scatter(UNUSUED const Ray &ray, const HitRecord &record, Color &emitted, Color &attenuation, Ray &scattered) const {
	emitted.zero();
	const Vec3 &normal = record.normal;
	attenuation = albedo->value(record.hittable, scattered.origin, normal);
	Scalar n = Random::real();
	const Scalar phi = Random::angle();
	const Scalar ax = std::abs(normal.x), ay = std::abs(normal.y), az = std::abs(normal.z);
	if(ax < ay && ax < az) {
		const Scalar nyz = normal.y*normal.y + normal.z*normal.z;
		const Scalar o = std::sqrt(n / nyz);
		const Scalar co = o * std::cos(phi), si = o * std::sin(phi);
		n = std::sqrt(1. - n);
		scattered.direction.x = n * normal.x +                 si * nyz;
		scattered.direction.y = n * normal.y + co * normal.z - si * normal.y * normal.x;
		scattered.direction.z = n * normal.z - co * normal.y - si * normal.z * normal.x;
	} else if(ay < az) {
		const Scalar nzx = normal.z*normal.z + normal.x*normal.x;
		const Scalar o = std::sqrt(n / nzx);
		const Scalar co = o * std::cos(phi), si = o * std::sin(phi);
		n = std::sqrt(1. - n);
		scattered.direction.x = n * normal.x - co * normal.z - si * normal.x * normal.y;
		scattered.direction.y = n * normal.y +                 si * nzx;
		scattered.direction.z = n * normal.z + co * normal.x - si * normal.z * normal.y;
	} else {
		const Scalar nxy = normal.x*normal.x + normal.y*normal.y;
		const Scalar o = std::sqrt(n / nxy);
		const Scalar co = o * std::cos(phi), si = o * std::sin(phi);
		n = std::sqrt(1. - n);
		scattered.direction.x = n * normal.x + co * normal.y - si * normal.x * normal.z;
		scattered.direction.y = n * normal.y - co * normal.x - si * normal.y * normal.z;
		scattered.direction.z = n * normal.z +                 si * nxy;
	}
	return true;
}

Scalar Lambertian::scattering_pdf(const Ray &scattered, const Vec3 &normal) const {
	const Scalar cosTheta = dot(normal, scattered.direction);
	return cosTheta <= 0. ? 0. : cosTheta * (1. / M_PI);
}

bool Metal::scatter(const Ray &ray, const HitRecord &record, Color &emitted, Color &attenuation, Ray &scattered) const {
	emitted.zero();
	attenuation = albedo;
	scattered.direction = (reflect(ray.direction, record.normal) + fuzz * Vec3::randomBall()).normalized();
	return dot(scattered.direction, record.normal) > 0.;
}

bool Dielectric::scatter(const Ray &ray, const HitRecord &record, Color &emitted, Color &attenuation, Ray &scattered) const {
	emitted.zero();
	attenuation = Color(1., 1., 1.);
	const Vec3 &normal = record.normal;
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

bool DiffuseLight::scatter(const Ray &ray, const HitRecord &record, Color &emitted, UNUSUED Color &attenuation, Ray &scattered) const {
	if(dot(record.normal, ray.direction) < 0.) emitted = emit->value(record.hittable, scattered.origin, record.normal);
	else emitted.zero();
	return false;
}