#include "vec.h"

class AABB {
public:
	AABB() = default;
	AABB(const Vec3 &mini, const Vec3 &maxi): mini(mini), maxi(maxi) {}

	bool hit(const Ray &ray, Scalar tMax, Scalar &t) const;

	void surround(const AABB &other);

	const Vec3& min() const { return mini; }
	const Vec3& max() const { return maxi; }

private:
	Vec3 mini, maxi;
};