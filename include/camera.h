#include "vec.h"

class Camera {
public:
	Camera() = default;
	Camera(const Vec3 &pos, const Vec3 &direction, const Vec3 &up, Scalar fov, Scalar aspectRatio, Scalar aperture, Scalar focusDistance);

	Ray getRay(Scalar x, Scalar y) const;

private:
	Vec3 pos, corner, horizontal, vertical;
	Vec3 u, v;
	Scalar lensRadius;
};