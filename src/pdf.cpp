#include "pdf.h"

const UniformPDF uniformPDF;
const PDF* UniformPDF::instance = &uniformPDF;

Vec3 CosinePDF::generate(const Vec3 &normal) const {
	const Scalar cn = std::pow(Random::real(), 1. / (power + 1.));
	const Scalar phi = Random::angle();
	const Scalar ax = std::abs(normal.x), ay = std::abs(normal.y), az = std::abs(normal.z);
	if(ax < ay && ax < az) {
		const Scalar nyz = normal.y*normal.y + normal.z*normal.z;
		const Scalar o = std::sqrt((1. - cn*cn) / nyz);
		const Scalar co = o * std::cos(phi), si = o * std::sin(phi);
		return Vec3(
			cn * normal.x +                 si * nyz,
			cn * normal.y + co * normal.z - si * normal.y * normal.x,
			cn * normal.z - co * normal.y - si * normal.z * normal.x
		);
	} else if(ay < az) {
		const Scalar nzx = normal.z*normal.z + normal.x*normal.x;
		const Scalar o = std::sqrt((1. - cn*cn) / nzx);
		const Scalar co = o * std::cos(phi), si = o * std::sin(phi);
		return Vec3(
			cn * normal.x - co * normal.z - si * normal.x * normal.y,
			cn * normal.y +                 si * nzx,
			cn * normal.z + co * normal.x - si * normal.z * normal.y
		);
	} else {
		const Scalar nxy = normal.x*normal.x + normal.y*normal.y;
		const Scalar o = std::sqrt((1. - cn*cn) / nxy);
		const Scalar co = o * std::cos(phi), si = o * std::sin(phi);
		return Vec3(
			cn * normal.x + co * normal.y - si * normal.x * normal.z,
			cn * normal.y - co * normal.x - si * normal.y * normal.z,
			cn * normal.z +                 si * nxy
		);
	}
}