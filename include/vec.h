#pragma once

#include "random.h"

#include <ostream>

class Vec2 {
public:
	Scalar x, y;

	Vec2(Scalar x=0., Scalar y=0.): x(x), y(y) {}

	inline Vec2 operator*(Scalar scalar) const { return Vec2(x*scalar, y*scalar); }

	inline Scalar norm2() const { return x*x + y*y; }

	static Vec2 randomDisc() {
		Scalar alpha = 2. * M_PI * Random::real();
		Scalar r = std::sqrt(Random::real());
		return Vec2(r * std::cos(alpha), r * std::sin(alpha));
	}
};

class Vec3 {
private:
	Scalar X[3];
public:

	Vec3(Scalar x=0., Scalar y=0., Scalar z=0.) { X[0] = x; X[1] = y; X[2] = z; }

	inline const Scalar& x() const { return X[0]; }
	inline Scalar& x() { return X[0]; }
	inline const Scalar& y() const { return X[1]; }
	inline Scalar& y() { return X[1]; }
	inline const Scalar& z() const { return X[2]; }
	inline Scalar& z() { return X[2]; }
	inline const Scalar& operator[](int i) const { return X[i]; }
	inline Scalar& operator[](uint i) { return X[i]; }
	inline Scalar& operator[](int i) { return X[i]; }

	static Vec3 random() { return Vec3(Random::real(), Random::real(), Random::real()); }
	static Vec3 randomRange(Scalar a, Scalar b) {
		return Vec3(Random::realRange(a, b), Random::realRange(a, b), Random::realRange(a, b));
	}
	static Vec3 randomBall() {
		Scalar phi = 2. * M_PI * Random::real();
		Scalar cosTheta = 2. * Random::real() - 1.;
		Scalar sinTheta = std::sqrt(1. - cosTheta*cosTheta);
		Scalar r = std::cbrt(Random::real());
		return Vec3(r * sinTheta * std::cos(phi), r * sinTheta * std::sin(phi), r * cosTheta);
	}

	inline Vec3 operator-() const { return Vec3(-X[0], -X[1], -X[2]); }

	inline Vec3 operator+(const Vec3 &other) const { return Vec3(X[0]+other.X[0], X[1]+other.X[1], X[2]+other.X[2]); }
	inline Vec3 operator-(const Vec3 &other) const { return Vec3(X[0]-other.X[0], X[1]-other.X[1], X[2]-other.X[2]); }
	inline Vec3 operator*(const Vec3 &other) const { return Vec3(X[0]*other.X[0], X[1]*other.X[1], X[2]*other.X[2]); }

	inline Vec3& operator+=(const Vec3 &other) {
		X[0] += other.X[0];
		X[1] += other.X[1];
		X[2] += other.X[2];
		return *this;
	}

	inline Vec3 operator/(Scalar scalar) const { return Vec3(X[0]/scalar, X[1]/scalar, X[2]/scalar); }
	inline Vec3 operator*(Scalar scalar) const { return Vec3(X[0]*scalar, X[1]*scalar, X[2]*scalar); }

	inline Vec3& operator*=(Scalar scalar) {
		X[0] *= scalar;
		X[1] *= scalar;
		X[2] *= scalar;
		return *this;
	}
	inline Vec3& operator/=(Scalar scalar) {
		X[0] /= scalar;
		X[1] /= scalar;
		X[2] /= scalar;
		return *this;
	}

	inline Scalar norm() const { return std::sqrt(X[0]*X[0] + X[1]*X[1] + X[2]*X[2]); }
	inline Scalar norm2() const { return X[0]*X[0] + X[1]*X[1] + X[2]*X[2]; }
	inline Vec3 normalized() const { return *this / std::sqrt(X[0]*X[0] + X[1]*X[1] + X[2]*X[2]); }

	inline Scalar minCoeff() const { return std::min(X[0], std::min(X[1], X[2])); }
	inline Scalar maxCoeff() const { return std::max(X[0], std::max(X[1], X[2])); }
};

inline Vec3 operator*(Scalar scalar, const Vec3 &v) { return Vec3(scalar*v.x(), scalar*v.y(), scalar*v.z()); }
inline Vec3 operator/(Scalar scalar, const Vec3 &v) { return Vec3(scalar/v.x(), scalar/v.y(), scalar/v.z()); }

inline Scalar dot(const Vec3 &u, const Vec3 &v) { return u.x()*v.x() + u.y()*v.y() + u.z()*v.z(); }
inline Vec3 cross(const Vec3 &u, const Vec3 &v) { return Vec3(u.y()*v.z()-u.z()*v.y(), u.z()*v.x()-u.x()*v.z(), u.x()*v.y()-u.y()*v.x()); }

inline Vec3 min(const Vec3 &u, const Vec3 &v) { return Vec3(std::min(u.x(), v.x()), std::min(u.y(), v.y()), std::min(u.z(), v.z())); }
inline Vec3 max(const Vec3 &u, const Vec3 &v) { return Vec3(std::max(u.x(), v.x()), std::max(u.y(), v.y()), std::max(u.z(), v.z())); }

inline Vec3 reflect(const Vec3 &v, const Vec3 &n) { return v - (2. * dot(v, n)) * n; }

inline std::ostream& operator<<(std::ostream &stream, const Vec3 &v) {
	return stream << v.x() << ' ' << v.y() << ' ' << v.z();
}

typedef Vec3 Color;

class Ray {
public:
	Ray() = default;
	Ray(const Vec3 &origin, const Vec3 &direction): _origin(origin), _direction(direction) {}
	inline const Vec3& origin() const { return _origin; }
	inline const Vec3& direction() const { return _direction; }
	inline Vec3 at(Scalar t) const { return _origin + t * _direction; }
private:
	Vec3 _origin, _direction;
};