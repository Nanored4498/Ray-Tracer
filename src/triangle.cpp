#include "triangle.h"

#include "stats.h"
#include <fstream>

std::atomic<unsigned long long> Stats::triangleRayTest = {0uLL};
thread_local unsigned long long Stats::localTriangleRayTest = 0uLL;

Triangle::Triangle(const Vec3 &a, const Vec3 &b, const Vec3 &c, std::shared_ptr<const Material> material, bool biface):
	material(material),
	biface(biface) {
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
	UPDATE_TRIANGLE_STATS
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
			record.normal = biface && dot(normal, ray.direction()) > 0. ? -normal : normal;
			record.material = material.get();
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
			record.normal = biface && dot(normal, ray.direction()) > 0. ? -normal : normal;
			record.material = material.get();
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
			record.normal = biface && dot(normal, ray.direction()) > 0. ? -normal : normal;
			record.material = material.get();
			record.t = t;
			return true;
		} else return false;
	}
	throw std::runtime_error("Non reachable code in Triangle hit!");
}

void loadOBJ(const std::string &fileName, HittableList &list, const Vec3 &rotAxis, Scalar angle, Scalar scale, const Vec3 &pos) {
	std::ifstream ifs(fileName);
	std::string word;
	std::vector<Vec3> vertices;
	std::vector<std::tuple<uint, uint, uint>> faces;
	AABB box;
	while(ifs >> word) {
		if(word[0] == '#') {
			std::getline(ifs, word);
			continue;
		}
		if(word.size() != 1) throw std::runtime_error("Unknown word " + word + " in OBJ file!");
		if(word[0] == 'v') {
			vertices.emplace_back();
			ifs >> vertices.back().x() >> vertices.back().y() >> vertices.back().z();
			if(vertices.size() == 1) box = AABB(vertices.back(), vertices.back());
			else box.surround(AABB(vertices.back(), vertices.back()));
		} else if(word[0] == 'f') {
			faces.emplace_back();
			ifs >> std::get<0>(faces.back()) >> std::get<1>(faces.back()) >> std::get<2>(faces.back());
		} else throw std::runtime_error("Unknown word " + word + " in OBJ file!");
	}
	ifs.close();

	Vec3 boxMid = .5 * (box.min() + box.max());
	double scale0 = 1. / (box.max() - box.min()).maxCoeff();
	Vec3 z = rotAxis.normalized();
	Vec3 x = Vec3::random();
	x = (x - dot(x, z) * z).normalized();
	Vec3 y = cross(z, x);
	angle *= M_PI / 180.;
	Scalar co = std::cos(angle), si = std::sin(angle);
	for(Vec3 &v : vertices) {
		v = (v - boxMid) * scale0;
		Scalar vx = dot(v, x), vy = dot(v, y);
		v = pos + scale * (dot(v, z) * z + (vx * co - vy * si) * x + (vx * si + vy * co) * y);
	}

	for(const auto &[i, j, k] : faces)
		// list.add(new Triangle(vertices[i-1], vertices[j-1], vertices[k-1], sharedMat));
		list.add(new Triangle(vertices[i-1], vertices[j-1], vertices[k-1], std::make_shared<Metal>(Vec3(.52+.04*Random::real(), .35+.03*Random::real(), .05+.01*Random::real()), .08*Random::real())));
}