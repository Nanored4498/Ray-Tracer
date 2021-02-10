#pragma once

#include <random>
#include <chrono>

class Random {
public:
	static void init() { re.seed(std::chrono::system_clock::now().time_since_epoch().count()); }

	static double real() { return real_unif(re); }
	static double realRange(double a, double b) { return a + (b-a) * real_unif(re); }

private:
	inline static std::default_random_engine re;
	inline static std::uniform_real_distribution<double> real_unif = std::uniform_real_distribution<double>(0., 1.);
};