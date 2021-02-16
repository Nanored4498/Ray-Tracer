#include "bvh.h"

#include <algorithm>

BVHNode::BVHNode(std::vector<std::shared_ptr<const Hittable>>::iterator start, size_t nb) {
	if(nb < 2) throw std::runtime_error("Not enougth hittables in BVHNode!");

	uint axis;
	auto comp = [&axis](const std::shared_ptr<const Hittable> &a, const std::shared_ptr<const Hittable> &b) {
		return a->boundingBox().max()[axis] < b->boundingBox().max()[axis];
	};

	uint bestAxis = 0;
	uint bestSep = 1;
	Scalar bestScore = std::numeric_limits<Scalar>::max();
	std::vector<Scalar> surfaces(nb-1);
	for(axis = 0; axis < 3; ++axis) {
		std::sort(start, start+nb, comp);
		uint i = 0;
		box = (*start)->boundingBox();
		while(true) {
			surfaces[i] = box.surface();
			if(++i < surfaces.size()) box.surround((*(start+i))->boundingBox());
			else break;
		}
		i = nb-1;
		box = (*(start+i))->boundingBox();
		while(true) {
			double score = box.surface() * (nb - i) + surfaces[i-1] * i;
			if(score < bestScore) {
				bestScore = score;
				bestAxis = axis;
				bestSep = i;
			}
			if(--i > 0) box.surround((*(start+i))->boundingBox());
			else break;
		}
	}

	if(bestAxis < 2) {
		axis = bestAxis;
		std::sort(start, start+nb, comp);
	}

	if(bestSep == 1) left = *start;
	else left = std::make_shared<BVHNode>(start, bestSep);
	if(bestSep+1 == nb) right = *(start + bestSep);
	else right = std::make_shared<BVHNode>(start + bestSep, nb-bestSep);

	box = left->boundingBox();
	box.surround(right->boundingBox());
}

bool BVHNode::hit(const Ray &ray, Scalar tMax, HitRecord &record) const {
	#ifdef BVH_STATS
	++ nbIntersections;
	#endif
	Scalar t0Left;
	if(left->hitBox(ray, tMax, t0Left)) {
		Scalar t0Right;
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