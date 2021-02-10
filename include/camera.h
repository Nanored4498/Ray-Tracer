#include "vec.h"

class Camera {
public:
	Camera(const Vec3 &pos, const Vec3 &direction, const Vec3 &up, double fov, double aspectRatio, double aperture, double focusDistance);

	Ray getRay(double x, double y) const;

private:
	Vec3 pos, corner, horizontal, vertical;
	Vec3 u, v;
	double lensRadius;
};