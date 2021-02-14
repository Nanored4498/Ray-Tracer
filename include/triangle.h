#include "hittable.h"

class Triangle : public Hittable {
public:
	Triangle(const Vec3 &a, const Vec3 &b, const Vec3 &c, std::shared_ptr<const Material> material, bool biface=false);

	~Triangle() {}
	
	bool hit(const Ray &ray, Scalar tMax, HitRecord &record) const override;

private:
	Vec3 normal;
	std::shared_ptr<const Material> material;
	bool biface;
	unsigned char fixedColumn;
	Scalar invT[9];
};

void loadOBJ(const std::string &fileName, HittableList &list, const Vec3 &rotAxis, Scalar angle, Scalar scale, const Vec3 &pos);