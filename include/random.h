#pragma once

#include "all.h"

#include <random>
#include <chrono>

class Random {
public:
	static void init() { re.seed(std::chrono::system_clock::now().time_since_epoch().count()); }

	static Scalar real() { return real_unif(re); }
	static Scalar realNeg() { return real_unif_neg(re); }
	static Scalar angle() { return real_unif_angle(re); }
	static Scalar realRange(const Scalar a, const Scalar b) { return a + (b-a) * real_unif(re); }
	static int intRange(int a, int b) { return std::uniform_int_distribution<int>(a, b)(re); }

private:
	inline static thread_local std::default_random_engine re;
	inline static std::uniform_real_distribution<Scalar> real_unif = std::uniform_real_distribution<Scalar>(0., 1.);
	inline static std::uniform_real_distribution<Scalar> real_unif_neg = std::uniform_real_distribution<Scalar>(-1., 1.);
	inline static std::uniform_real_distribution<Scalar> real_unif_angle = std::uniform_real_distribution<Scalar>(0., 2. * M_PI);
};