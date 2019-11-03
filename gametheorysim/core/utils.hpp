#pragma once
#include <random>
#include <chrono>
#define DO_ONCE ([](){static char o = 0;return !o &&++o;}())

#define M_PI 3.14159265358979323846264338327950288

namespace utils {
	int random(int min, int max);
	float random(float min, float max);
}