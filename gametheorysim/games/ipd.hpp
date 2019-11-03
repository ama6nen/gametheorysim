#pragma once
#include "../imgui/imgui.h"
#include <vector>
#include <algorithm>
#include <string>
#include "../logic/pfmatrix.hpp"
#include "../imgui/imgui_plot.h"
#include "../core/utils.hpp"

namespace ipd {
	extern int iterations;
	extern pfmatrix matrix;
	void setup();
	void simulation();
	void menu();
}