#pragma once

#include "all.h"

#include <random>
#include <chrono>

class Random {
public:
	inline static void init(const int t) { re.seed(std::chrono::system_clock::now().time_since_epoch().count() + 123*t); }

	inline static Scalar real() { return std::uniform_real_distribution<Scalar>(0., 1.)(re); }
	inline static Scalar realNeg() { return std::uniform_real_distribution<Scalar>(-1., 1.)(re); }
	inline static Scalar angle() { return std::uniform_real_distribution<Scalar>(0., 2.*M_PI)(re); }
	inline static Scalar realRange(const Scalar a, const Scalar b) { return std::uniform_real_distribution<Scalar>(a, b)(re); }
	inline static int intRange(int a, int b) { return std::uniform_int_distribution<int>(a, b)(re); }

private:
	inline static thread_local std::mt19937 re;
};