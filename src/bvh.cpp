#include "bvh.h"

#include <algorithm>

BVHNode::BVHNode(std::vector<std::shared_ptr<Hittable>>::iterator start, std::vector<std::shared_ptr<Hittable>>::iterator end) {
	int axis = Random::intRange(0, 2);
	auto comp = [axis](const std::shared_ptr<Hittable> &a, const std::shared_ptr<Hittable> &b) {
		return a->boundingBox().max()[axis] < b->boundingBox().max()[axis];
	};

	if(start + 2 > end) throw std::runtime_error("Not enougth hittables in BVHNode!");
	std::sort(start, end, comp);
	std::vector<std::shared_ptr<Hittable>>::iterator mid = start + (end - start) / 2;
	if(start+1==mid) left = *start;
	else left = std::make_shared<BVHNode>(start, mid);
	if(mid+1==end) right = *mid;
	else right = std::make_shared<BVHNode>(mid, end);

	box = left->boundingBox();
	box.surround(right->boundingBox());
}

bool BVHNode::hit(const Ray &ray, double tMax, HitRecord &record) const {
	#ifdef BVH_STATS
	++ nbIntersections;
	#endif
	double t0Left;
	if(left->hitBox(ray, tMax, t0Left)) {
		double t0Right;
		if(right->hitBox(ray, tMax, t0Right)) {
			if(t0Left < t0Right) {
				if(left->hit(ray, tMax, record)) {
					if(record.t > t0Right) right->hit(ray, record.t, record);
					return true;
				} else return right->hit(ray, tMax, record);
			} else {
				if(right->hit(ray, tMax, record)) {
					if(record.t > t0Left) left->hit(ray, record.t, record);
					return true;
				} else return left->hit(ray, tMax, record);
			}
		} else return left->hit(ray, tMax, record);
	} else return right->hit(ray, tMax, record);
}