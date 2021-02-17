#include "aabb.h"

bool AABB::hit(const Ray &ray, Scalar tMax, Scalar &t) const {
	t = EPS;
	Scalar invD = 1. / ray.direction().x(), t0, t1;
	if(invD < 0.) {
		t0 = invD * (maxi.x() - ray.origin().x());
		t1 = invD * (mini.x() - ray.origin().x());
	} else {
		t0 = invD * (mini.x() - ray.origin().x());
		t1 = invD * (maxi.x() - ray.origin().x());
	}
	if(t0 > t) t = t0;
	if(t1 < tMax) tMax = t1;
	if(tMax <= t) return false;
	invD = 1. / ray.direction().y();
	if(invD < 0.) {
		t0 = invD * (maxi.y() - ray.origin().y());
		t1 = invD * (mini.y() - ray.origin().y());
	} else {
		t0 = invD * (mini.y() - ray.origin().y());
		t1 = invD * (maxi.y() - ray.origin().y());
	}
	if(t0 > t) t = t0;
	if(t1 < tMax) tMax = t1;
	if(tMax <= t) return false;
	invD = 1. / ray.direction().z();
	if(invD < 0.) {
		t0 = invD * (maxi.z() - ray.origin().z());
		t1 = invD * (mini.z() - ray.origin().z());
	} else {
		t0 = invD * (mini.z() - ray.origin().z());
		t1 = invD * (maxi.z() - ray.origin().z());
	}
	if(t0 > t) t = t0;
	if(t1 < tMax) tMax = t1;
	return t < tMax;
}

bool AABB::hitInv(const Ray &ray, Scalar tMax, Scalar &t) const {
	t = EPS;
	Scalar t0, t1;
	if(ray.direction().x() < 0.) {
		t0 = ray.direction().x() * (maxi.x() - ray.origin().x());
		t1 = ray.direction().x() * (mini.x() - ray.origin().x());
	} else {
		t0 = ray.direction().x() * (mini.x() - ray.origin().x());
		t1 = ray.direction().x() * (maxi.x() - ray.origin().x());
	}
	if(t0 > t) t = t0;
	if(t1 < tMax) tMax = t1;
	if(tMax <= t) return false;
	if(ray.direction().y() < 0.) {
		t0 = ray.direction().y() * (maxi.y() - ray.origin().y());
		t1 = ray.direction().y() * (mini.y() - ray.origin().y());
	} else {
		t0 = ray.direction().y() * (mini.y() - ray.origin().y());
		t1 = ray.direction().y() * (maxi.y() - ray.origin().y());
	}
	if(t0 > t) t = t0;
	if(t1 < tMax) tMax = t1;
	if(tMax <= t) return false;
	if(ray.direction().z() < 0.) {
		t0 = ray.direction().z() * (maxi.z() - ray.origin().z());
		t1 = ray.direction().z() * (mini.z() - ray.origin().z());
	} else {
		t0 = ray.direction().z() * (mini.z() - ray.origin().z());
		t1 = ray.direction().z() * (maxi.z() - ray.origin().z());
	}
	if(t0 > t) t = t0;
	if(t1 < tMax) tMax = t1;
	return t < tMax;
}


void AABB::surround(const AABB &other) {
	for(int i = 0; i < 3; ++i) {
		if(other.mini[i] < mini[i]) mini[i] = other.mini[i];
		if(other.maxi[i] > maxi[i]) maxi[i] = other.maxi[i];
	}
}