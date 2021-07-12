#pragma once

#include "random.h"

#include <ostream>

class Vec2 {
public:
	Scalar x, y;

	Vec2(Scalar x=0., Scalar y=0.): x(x), y(y) {}

	inline const Scalar& operator[](uint i) const { return reinterpret_cast<const Scalar*>(this)[i]; }
	inline const Scalar& operator[](int i) const { return reinterpret_cast<const Scalar*>(this)[i]; }
	inline Scalar& operator[](uint i) { return reinterpret_cast<Scalar*>(this)[i]; }
	inline Scalar& operator[](int i) { return reinterpret_cast<Scalar*>(this)[i]; }

	inline Vec2 operator*(Scalar scalar) const { return Vec2(x*scalar, y*scalar); }

	inline Scalar norm2() const { return x*x + y*y; }

	static inline Vec2 randomDisc() {
		const Scalar alpha = Random::angle();
		const Scalar r = std::sqrt(Random::real());
		return Vec2(r * std::cos(alpha), r * std::sin(alpha));
	}

	static inline Vec2 randomDisc(Scalar R) {
		const Scalar alpha = Random::angle();
		const Scalar r = R * std::sqrt(Random::real());
		return Vec2(r * std::cos(alpha), r * std::sin(alpha));
	}
};

inline std::ostream& operator<<(std::ostream &stream, const Vec2 &v) {
	return stream << v.x << ' ' << v.y;
}

class Vec3 {
public:
	Scalar x, y, z;

	Vec3(Scalar x=0., Scalar y=0., Scalar z=0.): x(x), y(y), z(z) {}

	inline void zero() { x = y = z = 0.; }

	inline const Scalar& operator[](uint i) const { return reinterpret_cast<const Scalar*>(this)[i]; }
	inline const Scalar& operator[](int i) const { return reinterpret_cast<const Scalar*>(this)[i]; }
	inline Scalar& operator[](uint i) { return reinterpret_cast<Scalar*>(this)[i]; }
	inline Scalar& operator[](int i) { return reinterpret_cast<Scalar*>(this)[i]; }

	static Vec3 random() { return Vec3(Random::real(), Random::real(), Random::real()); }
	static Vec3 randomRange(Scalar a, Scalar b) {
		return Vec3(Random::realRange(a, b), Random::realRange(a, b), Random::realRange(a, b));
	}
	static inline Vec3 randomBall() {
		const Scalar phi = Random::angle();
		const Scalar cosTheta = Random::realNeg();
		const Scalar sinTheta = std::sqrt(1. - cosTheta*cosTheta);
		const Scalar r = std::cbrt(Random::real());
		return Vec3(r * sinTheta * std::cos(phi), r * sinTheta * std::sin(phi), r * cosTheta);
	}
	static inline Vec3 randomSphere() {
		const Scalar phi = Random::angle();
		const Scalar cosTheta = Random::realNeg();
		const Scalar sinTheta = std::sqrt(1. - cosTheta*cosTheta);
		return Vec3(sinTheta * std::cos(phi), sinTheta * std::sin(phi), cosTheta);
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
	inline Vec3& operator-=(const Vec3 &other) {
		x -= other.x;
		y -= other.y;
		z -= other.z;
		return *this;
	}
	inline Vec3& operator*=(const Vec3 &other) {
		x *= other.x;
		y *= other.y;
		z *= other.z;
		return *this;
	}

	inline Vec3 operator/(Scalar scalar) const { return Vec3(x/scalar, y/scalar, z/scalar); }
	inline Vec3 operator*(Scalar scalar) const { return Vec3(x*scalar, y*scalar, z*scalar); }

	inline Vec3& operator*=(Scalar scalar) {
		x *= scalar;
		y *= scalar;
		z *= scalar;
		return *this;
	}
	inline Vec3& operator/=(Scalar scalar) {
		x /= scalar;
		y /= scalar;
		z /= scalar;
		return *this;
	}

	inline bool operator!=(const Vec3 &other) const { return x != other.x || y != other.y || z != other.z; }

	inline Scalar norm() const { return std::sqrt(x*x + y*y + z*z); }
	inline Scalar norm2() const { return x*x + y*y + z*z; }
	inline Vec3 normalized() const { return *this / std::sqrt(x*x + y*y + z*z); }

	inline Scalar minCoeff() const { return std::min(x, std::min(y, z)); }
	inline Scalar maxCoeff() const { return std::max(x, std::max(y, z)); }

	inline friend Scalar dot(const Vec3 &u, const Vec3 &v) { return u.x*v.x + u.y*v.y + u.z*v.z; }
	inline friend Vec3 cross(const Vec3 &u, const Vec3 &v) { return Vec3(u.y*v.z-u.z*v.y, u.z*v.x-u.x*v.z, u.x*v.y-u.y*v.x); }
};

inline Vec3 operator*(Scalar scalar, const Vec3 &v) { return Vec3(scalar*v.x, scalar*v.y, scalar*v.z); }
inline Vec3 operator/(Scalar scalar, const Vec3 &v) { return Vec3(scalar/v.x, scalar/v.y, scalar/v.z); }

inline Vec3 min(const Vec3 &u, const Vec3 &v) { return Vec3(std::min(u.x, v.x), std::min(u.y, v.y), std::min(u.z, v.z)); }
inline Vec3 max(const Vec3 &u, const Vec3 &v) { return Vec3(std::max(u.x, v.x), std::max(u.y, v.y), std::max(u.z, v.z)); }

inline Vec3 reflect(const Vec3 &v, const Vec3 &n) { return v - (2. * dot(v, n)) * n; }

inline std::ostream& operator<<(std::ostream &stream, const Vec3 &v) {
	return stream << v.x << ' ' << v.y << ' ' << v.z;
}

typedef Vec3 Color;

class Ray {
public:
	Vec3 origin, direction;

	Ray() = default;
	Ray(const Vec3 &origin, const Vec3 &direction): origin(origin), direction(direction) {}
	inline Vec3 at(Scalar t) const { return origin + t * direction; }
};