#pragma once

#include "random.h"

class Vec2 {
public:
	double x, y;

	Vec2(double x=0., double y=0.): x(x), y(y) {}

	inline Vec2 operator*(double scalar) const { return Vec2(x*scalar, y*scalar); }

	inline double norm2() const { return x*x + y*y; }

	static Vec2 randomDisc() {
		double alpha = 2. * M_PI * Random::real();
		double r = std::sqrt(Random::real());
		return Vec2(r * std::cos(alpha), r * std::sin(alpha));
	}
};

class Vec3 {
public:
	double x, y, z;

	Vec3(double x=0., double y=0., double z=0.): x(x), y(y), z(z) {}

	static Vec3 random() { return Vec3(Random::real(), Random::real(), Random::real()); }
	static Vec3 randomRange(double a, double b) {
		return Vec3(Random::realRange(a, b), Random::realRange(a, b), Random::realRange(a, b));
	}
	static Vec3 randomBall() {
		double phi = 2. * M_PI * Random::real();
		double cosTheta = 2. * Random::real() - 1.;
		double sinTheta = std::sqrt(1. - cosTheta*cosTheta);
		double r = std::cbrt(Random::real());
		return Vec3(r * sinTheta * std::cos(phi), r * sinTheta * std::sin(phi), r * cosTheta);
	}

	inline Vec3 operator-() const { return Vec3(-x, -y, -z); }

	inline Vec3 operator+(const Vec3 &other) const { return Vec3(x+other.x, y+other.y, z+other.z); }
	inline Vec3 operator-(const Vec3 &other) const { return Vec3(x-other.x, y-other.y, z-other.z); }
	inline Vec3 operator*(const Vec3 &other) const { return Vec3(x*other.x, y*other.y, z*other.z); }

	inline Vec3& operator+=(const Vec3 &other) {
		x += other.x;
		y += other.y;
		z += other.z;
		return *this;
	}

	inline Vec3 operator/(double scalar) const { return Vec3(x/scalar, y/scalar, z/scalar); }
	inline Vec3 operator*(double scalar) const { return Vec3(x*scalar, y*scalar, z*scalar); }

	inline Vec3& operator/=(double scalar) {
		x /= scalar;
		y /= scalar;
		z /= scalar;
		return *this;
	}

	inline double norm() const { return std::sqrt(x*x + y*y + z*z); }
	inline double norm2() const { return x*x + y*y + z*z; }
	inline Vec3 normalized() const { return *this / std::sqrt(x*x + y*y + z*z); }
};

inline Vec3 operator*(double scalar, const Vec3 &v) { return Vec3(scalar*v.x, scalar*v.y, scalar*v.z); }

inline double dot(const Vec3 &u, const Vec3 &v) { return u.x*v.x + u.y*v.y + u.z*v.z; }
inline Vec3 cross(const Vec3 &u, const Vec3 &v) { return Vec3(u.y*v.z-u.z*v.y, u.z*v.x-u.x*v.z, u.x*v.y-u.y*v.x); }

inline Vec3 reflect(const Vec3 &v, const Vec3 &n) { return v - (2. * dot(v, n)) * n; }

typedef Vec3 Color;

class Ray {
public:
	Ray() = default;
	Ray(const Vec3 &origin, const Vec3 &direction): _origin(origin), _direction(direction) {}
	inline const Vec3& origin() const { return _origin; }
	inline const Vec3& direction() const { return _direction; }
	inline Vec3 at(double t) const { return _origin + t * _direction; }
private:
	Vec3 _origin, _direction;
};