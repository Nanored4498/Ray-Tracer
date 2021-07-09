#include "camera.h"

Camera::Camera(const Vec3 &pos, const Vec3 &direction, const Vec3 &up, Scalar fov, Scalar aspectRatio, Scalar aperture, Scalar focusDistance): pos(pos) {
	fov *= M_PI / 180.;
	const Scalar width = 2. * std::tan(fov / 2.);
	const Scalar height = width / aspectRatio;
	const Vec3 w = direction.normalized();

	u = cross(w, up);
	u /= u.norm();
	v = cross(u, w);

	horizontal = (width * focusDistance) * u;
	vertical = (height * focusDistance) * v;
	corner = pos + focusDistance * w - .5 * horizontal - .5 * vertical;

	lensRadius = .5 * aperture;
}

Ray Camera::getRay(Scalar x, Scalar y) const {
	const Vec2 offset = Vec2::randomDisc(lensRadius);
	const Vec3 origin = pos + offset.x * u + offset.y * v;
	return Ray(origin, (corner + x * horizontal + y * vertical - origin).normalized());
}