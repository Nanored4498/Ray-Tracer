#include "bvh.h"

#include <algorithm>

template<int i>
bool boxComp(const std::shared_ptr<Hittable> &a, const std::shared_ptr<Hittable> &b) {
	AABB boxA, boxB;
	if(!a->boundingBox(boxA) || !b->boundingBox(boxB))
		throw std::runtime_error("No bounding box in BVHNode comparator function!");
	return boxA.maxi[i] < boxB.maxi[i];
}

BVHNode::BVHNode(std::vector<std::shared_ptr<Hittable>>::iterator start, std::vector<std::shared_ptr<Hittable>>::iterator end) {
	int axis = Random::intRange(0, 2);
	auto comp = [axis](const std::shared_ptr<Hittable> &a, const std::shared_ptr<Hittable> &b) {
		AABB boxA, boxB;
		if(!a->boundingBox(boxA) || !b->boundingBox(boxB))
			throw std::runtime_error("No bounding box in BVHNode comparator function!");
		return boxA.max()[axis] < boxB.max()[axis];
	};

	if(start + 2 > end) throw std::runtime_error("Not enougth hittables in BVHNode!");
	std::sort(start, end, comp);
	std::vector<std::shared_ptr<Hittable>>::iterator mid = start + (end - start) / 2;
	if(start+1==mid) left = *start;
	else left = std::make_shared<BVHNode>(start, mid);
	if(mid+1==end) right = *mid;
	else right = std::make_shared<BVHNode>(mid, end);

	if(!left->boundingBox(box)) throw std::runtime_error("No bounding box in BVHNode left child!");
	AABB boxRight;
	if(!right->boundingBox(boxRight)) throw std::runtime_error("No bounding box in BVHNode right child!");
	box.surround(boxRight);
}

bool BVHNode::hit(const Ray &ray, double tMax, HitRecord &record) const {
	#ifdef BVH_STATS
	++ nbIntersections;
	#endif
	if(!box.hit(ray, tMax)) return false;
	if(left->hit(ray, tMax, record)) {
		right->hit(ray, record.t, record);
		return true;
	} else return right->hit(ray, tMax, record);
}