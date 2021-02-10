#include "vec.h"

class AABB {
public:
	AABB() = default;
	AABB(const Vec3 &mini, const Vec3 &maxi): mini(mini), maxi(maxi) {}

	bool hit(const Ray &ray, double tMax) const;

	void surround(const AABB &other);

private:
	Vec3 mini, maxi;
};