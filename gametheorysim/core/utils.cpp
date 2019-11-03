#pragma once
#include "utils.hpp"

std::mt19937 rng;
int utils::random(int min, int max)
{
	if (DO_ONCE)
		rng.seed((unsigned int)std::chrono::high_resolution_clock::now().time_since_epoch().count());
	std::uniform_int_distribution<int> distribution(min, max);
	return distribution(rng);
}

float utils::random(float min, float max)
{
	if (DO_ONCE)
		rng.seed((unsigned int)std::chrono::high_resolution_clock::now().time_since_epoch().count());
	std::uniform_real_distribution<float> distribution(min, max);
	return  distribution(rng);
}
