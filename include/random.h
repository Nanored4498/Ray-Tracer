#pragma once

#include "all.h"

#include <random>
#include <chrono>

class Random {
public:
	static void init() { re.seed(std::chrono::system_clock::now().time_since_epoch().count()); }

	static Scalar real() { return real_unif(re); }
	static Scalar realRange(Scalar a, Scalar b) { return a + (b-a) * real_unif(re); }
	static int intRange(int a, int b) { return std::uniform_int_distribution<int>(a, b)(re); }

private:
	inline static std::default_random_engine re;
	inline static std::uniform_real_distribution<Scalar> real_unif = std::uniform_real_distribution<Scalar>(0., 1.);
};