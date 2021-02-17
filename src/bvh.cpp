#include "bvh.h"

#include "stats.h"
#include <algorithm>
#include <queue>
#include <iostream>

std::atomic<unsigned long long> Stats::nodeRayTest = {0uLL};
thread_local unsigned long long Stats::localNodeRayTest = 0uLL;

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
	UPDATE_NODE_STATS
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


typedef std::pair<Scalar, const Hittable*> QElement;
struct qComp {
	inline bool operator()(const QElement &a, const QElement &b) const {
		return a.first > b.first;
	}
};

bool BVHTree::hit(const Ray &ray, Scalar tMax, HitRecord &record) const {
	// return root->hit(ray, tMax, record);
	Ray rayInv(ray.origin(), 1. / ray.direction());
	bool anyHit = false;
	std::priority_queue<QElement, std::vector<QElement>, qComp> Q;
	double t;
	if(root->left->hitBoxInv(rayInv, tMax, t)) Q.emplace(t, root->left.get());
	if(root->right->hitBoxInv(rayInv, tMax, t)) Q.emplace(t, root->right.get());
	while(!Q.empty()) {
		if(Q.top().first >= tMax) break;
		const Hittable* h = Q.top().second;
		Q.pop();
		double t0 = Q.empty() ? std::numeric_limits<Scalar>::max() : Q.top().first;
		// while(h->isNode()) {
		// 	#ifdef BVH_STATS
		// 	++ BVHNode::local_nbIntersections;
		// 	#endif
		// 	const BVHNode *node = (const BVHNode*) h;
		// 	if(node->left->hitBoxInv(rayInv, tMax, t)) {
		// 		double t2;
		// 		if(node->right->hitBoxInv(rayInv, tMax, t2)) {
		// 			if(t < t2) {
		// 				h = node->left.get();
		// 				Q.emplace(t2, node->right.get());
		// 			} else {
		// 				h = node->right.get();
		// 				Q.emplace(t, node->left.get());
		// 			}
		// 		} else h = node->left.get();
		// 	} else h = node->right.get();
		// }

		while(h->isNode()) {
			UPDATE_NODE_STATS
			const BVHNode *node = (const BVHNode*) h;
			if(node->left->hitBoxInv(rayInv, tMax, t)) {
				double t2;
				if(node->right->hitBoxInv(rayInv, tMax, t2)) {
					if(t < t2) {
						Q.emplace(t2, node->right.get());
						if(t <= t0) {
							h = node->left.get();
							if(t2 < t0) t0 = t2;
						} else {
							Q.emplace(t, node->left.get());
							break;
						}
					} else {
						Q.emplace(t, node->left.get());
						if(t2 <= t0) {
							h = node->right.get();
							if(t < t0) t0 = t;
						} else {
							Q.emplace(t2, node->right.get());
							break;
						}
					}
				} else {
					if(t <= t0) h = node->left.get();
					else {
						Q.emplace(t, node->left.get());
						break;
					}
				}
			} else {
				if(node->right->hitBoxInv(rayInv, tMax, t)) {
					if(t <= t0) h = node->right.get();
					else {
						Q.emplace(t, node->right.get());
						break;
					}
				} else break;

				// if(node->right->isNode()) {
				// 	node = (const BVHNode*) node->right.get();
				// 	if(node->left->hitBoxInv(rayInv, tMax, t)) Q.emplace(t, node->left.get());
				// 	if(node->right->hitBoxInv(rayInv, tMax, t)) Q.emplace(t, node->right.get());
				// } else h = node->right.get();
				// break;
			}
		}

		if(!h->isNode() && h->hit(ray, tMax, record)) {
			anyHit = true;
			tMax = record.t;
		}
	}
	return anyHit;
}