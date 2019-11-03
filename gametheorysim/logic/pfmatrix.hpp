#pragma once
#include "../imgui/imgui.h"
#include <string>
#include <vector>

class payoff { //payoff will be accessed from a multidimensional array payoff[n][m] and first is payoff for n, second m
public:
	int l;
	int r;

	payoff(int f, int s) {
		l = f;
		r = s;
	}
};

class item {
public:
	int count;
	std::string name;
	item(std::string name, int count) {
		this->name = name;
		this->count = count;
	}
};

class pfmatrix {
public:
	std::vector<item> items;
	std::vector<std::vector<payoff>> payoffs;
	void show_matrix(float align = 50.f, int bounds = 10);
};