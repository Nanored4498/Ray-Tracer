#include "aabb.h"

bool AABB::hit(const Ray &ray, double tMax, double &t) const {
	t = 1e-5;
	for(int i = 0; i < 3; ++i) {
		double invD = 1. / ray.direction()[i], t0, t1;
		if(invD < 0.) {
			t0 = invD * (maxi[i] - ray.origin()[i]);
			t1 = invD * (mini[i] - ray.origin()[i]);
		} else {
			t0 = invD * (mini[i] - ray.origin()[i]);
			t1 = invD * (maxi[i] - ray.origin()[i]);
		}
		if(t0 > t) t = t0;
		if(t1 < tMax) tMax = t1;
		if(tMax <= t) return false;
	}
	return true;
}


void AABB::surround(const AABB &other) {
	for(int i = 0; i < 3; ++i) {
		if(other.mini[i] < mini[i]) mini[i] = other.mini[i];
		if(other.maxi[i] > maxi[i]) maxi[i] = other.maxi[i];
	}
}