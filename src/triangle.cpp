#include "triangle.h"

#include <fstream>

Triangle::Triangle(const Vec3 &a, const Vec3 &b, const Vec3 &c, std::shared_ptr<const Material> material):
	material(material) {
	box = AABB(min(a, min(b, c)), max(a, max(b, c)));
	
	Vec3 e1 = b - a, e2 = c - a;
	normal = cross(e1, e2);

	if(std::abs(normal.x()) > std::abs(normal.y()) && std::abs(normal.x()) > std::abs(normal.z())) fixedColumn = 0;
	else if(std::abs(normal.y()) > std::abs(normal.z())) fixedColumn = 1;
	else if(std::abs(normal.z()) > 0.) fixedColumn = 2;
	else throw std::runtime_error("Degenerated triangle!");

	Scalar in = 1. / normal[fixedColumn];
	uint y = (fixedColumn+1) % 3;
	uint z = (fixedColumn+2) % 3;

	invT[0] = e2[z] * in;
	invT[1] = - e2[y] * in;
	invT[2] = (c[y] * a[z] - c[z] * a[y]) * in;
		
	invT[3] = - e1[z] * in;
	invT[4] = e1[y] * in;
	invT[5] = - (b[y] * a[z] - b[z] * a[y]) * in;
	
	invT[6] = normal[y] * in;
	invT[7] = normal[z] * in;
	invT[8] = - dot(a, normal) * in;

	normal /= normal.norm();
}

bool Triangle::hit(const Ray &ray, Scalar tMax, HitRecord &record) const {
	if(fixedColumn == 0) {
		Scalar t = - (ray.origin().x() + invT[6] * ray.origin().y() + invT[7] * ray.origin().z() + invT[8])
					/ (ray.direction().x() + invT[6] * ray.direction().y() + invT[7] * ray.direction().z());
		if(t <= EPS || t >= tMax) return false;
		Scalar py = ray.origin().y() + t * ray.direction().y(), pz = ray.origin().z() + t * ray.direction().z();
		Scalar xg = invT[0] * py + invT[1] * pz + invT[2];
		if(xg < 0.) return false;
		Scalar yg = invT[3] * py + invT[4] * pz + invT[5];
		if(yg >= 0. && xg + yg <= 1.) {
			record.pos = Vec3(ray.origin().x() + t * ray.direction().x(), py, pz);
			record.normal = normal;
			record.material = &(*material);
			record.t = t;
			return true;
		} else return false;
	} else if(fixedColumn == 1) {
		Scalar t = - (invT[7] * ray.origin().x() + ray.origin().y() + invT[6] * ray.origin().z() + invT[8])
					/ (invT[7] * ray.direction().x() + ray.direction().y() + invT[6] * ray.direction().z());
		if(t <= EPS || t >= tMax) return false;
		Scalar px = ray.origin().x() + t * ray.direction().x(), pz = ray.origin().z() + t * ray.direction().z();
		Scalar xg = invT[1] * px + invT[0] * pz + invT[2];
		if(xg < 0.) return false;
		Scalar yg = invT[4] * px + invT[3] * pz + invT[5];
		if(yg >= 0. && xg + yg <= 1.) {
			record.pos = Vec3(px, ray.origin().y() + t * ray.direction().y(), pz);
			record.normal = normal;
			record.material = &(*material);
			record.t = t;
			return true;
		} else return false;
	} else {
		Scalar t = - (invT[6] * ray.origin().x() + invT[7] * ray.origin().y() + ray.origin().z() + invT[8])
					/ (invT[6] * ray.direction().x() + invT[7] * ray.direction().y() + ray.direction().z());
		if(t <= EPS || t >= tMax) return false;
		Scalar px = ray.origin().x() + t * ray.direction().x(), py = ray.origin().y() + t * ray.direction().y();
		Scalar xg = invT[0] * px + invT[1] * py + invT[2];
		if(xg < 0.) return false;
		Scalar yg = invT[3] * px + invT[4] * py + invT[5];
		if(yg >= 0. && xg + yg <= 1.) {
			record.pos = Vec3(px, py, ray.origin().z() + t * ray.direction().z());
			record.normal = normal;
			record.material = &(*material);
			record.t = t;
			return true;
		} else return false;
	}
	throw std::runtime_error("Non reachable code in Triangle hit!");
}

void loadOBJ(const std::string &fileName, HittableList &list, const Material *material) {
	std::ifstream ifs(fileName);
	std::string word;
	std::vector<Vec3> vertices;
	std::shared_ptr<const Material> sharedMat(material);
	while(ifs >> word) {
		if(word[0] == '#') {
			std::getline(ifs, word);
			continue;
		}
		if(word.size() != 1) throw std::runtime_error("Unknown word " + word + " in OBJ file!");
		if(word[0] == 'v') {
			vertices.emplace_back();
			ifs >> vertices.back().x() >> vertices.back().y() >> vertices.back().z();
			std::swap(vertices.back().x(), vertices.back().z());
			vertices.back().z() *= -1.;
			vertices.back() *= 12.;
			vertices.back() += Vec3(4., -.42, .8);
		} else if(word[0] == 'f') {
			uint i, j, k;
			ifs >> i >> j >> k;
			// list.add(new Triangle(vertices[i-1], vertices[j-1], vertices[k-1], sharedMat));
			list.add(new Triangle(vertices[i-1], vertices[j-1], vertices[k-1], std::make_shared<Metal>(Vec3(.5+.07*Random::real(), .35+.06*Random::real(), .05*(1. + Random::real())), .11*Random::real())));
		} else throw std::runtime_error("Unknown word " + word + " in OBJ file!");
	}
	ifs.close();
}