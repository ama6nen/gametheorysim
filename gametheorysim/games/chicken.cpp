#pragma once
#include "chicken.hpp"


float meet_chance = 50.f;
int start_res = 30;

namespace chicken {
	pfmatrix matrix = pfmatrix();
	int iterations = 30;
	ImGui::PlotConfig conf;

	constexpr size_t buf_size = 100;
	static float x_data[buf_size];
	static float y_data1[buf_size];
	static float y_data2[buf_size];

}

void generate_data() {
	for (size_t i = 0; i < chicken::buf_size; ++i) {
		float f = float(i);
		chicken::x_data[i] = f;
		chicken::y_data1[i] = f;
		chicken::y_data2[i] = 100.f-f;
	}
}


void chicken::setup()
{
	matrix.items = { { "Hawk", 10 }, { "Dove", 20 },{ "Test", 30 }};
	for (unsigned int i = 0; i < matrix.items.size(); i++) {
		std::vector<payoff> one = {};
		for (unsigned int j = 0; j < matrix.items.size(); j++)
			one.push_back({ 0, 0 });
		matrix.payoffs.push_back(one);
	}
	generate_data();

	static const float* y_data[] = { y_data1, y_data2 };
	static ImU32 colors[2] = { ImColor(0, 255, 0), ImColor(255, 0, 0) };
	static uint32_t selection_start = 0, selection_length = 0;

	conf.values.xs = x_data;
	conf.values.count = buf_size;
	conf.values.ys_list = y_data; // use ys_list to draw several lines simultaneously
	conf.values.ys_count = 2;
	conf.values.colors = colors;
	conf.scale.min = 0;
	conf.scale.max = 100;
	conf.tooltip.show = true;
	conf.tooltip.format = "%g: %8.4g\n%g: %8.4g"; //does not handle more than 2 items for now cuz i didnt fix this lol 
	conf.tooltip.many = true;
	conf.grid_x.show = true;
	conf.grid_x.size = 128;
	conf.grid_y.show = true;
	conf.grid_y.size = 50.f;
	conf.selection.show = true;
	conf.selection.start = &selection_start;
	conf.selection.length = &selection_length;
	conf.frame_size = ImVec2(buf_size * 4, 200);
}

class entity {
public:
	int n{}; //basically just the number that will be used for payoffs[n][m] as n or m depending
	int resources{}; //payoffs subtract/add from this. if its less than 0 you die.
	int id{};
	entity(int x, int unit, int id_) {
		n = x;
		resources = unit;
		id = id_;
	}

	bool operator==(const entity& en) const {
		return id == en.id;
	}
};
void chicken::simulation()
{
	int x = 0;
	std::vector<entity> entities{};
	int id = 0;
	for (auto item : matrix.items) {
		for (int n = 0; n < item.count; n++) {
			entities.push_back({ x, start_res, id });
			id++;
		}	
		x++;
	}
	
	for (int iter = 0; iter < iterations; iter++) {
		int n = 0;
		for (auto& ent1 : entities) {
			if (utils::random(0.f, 100.f) <= meet_chance) { //we actually will meet someone according to chances
				int rnd = utils::random(0, entities.size() - 1);
				if(rnd == n) //ghetto but we dont wanna loop forever because it would break when population size is 1
					rnd = utils::random(0, entities.size()-1);

				auto& ent2 = entities[rnd];
				auto payoff = matrix.payoffs[ent1.n][ent2.n];
				ent1.resources += payoff.l;
				ent2.resources += payoff.r;
			}
			n++;
		}
		
		std::vector<entity> removables{};
		for (auto& ent1 : entities)
			if (ent1.resources <= 0) //remove dead entities
				removables.push_back(ent1);
		for (auto& removable : removables)
			entities.erase(std::remove(entities.begin(), entities.end(), removable), entities.end());
		
		int hawks = 0;
		int doves = 0;
		for (auto entity : entities) {
			if (entity.n == 0)
				hawks++;
			else
				doves++;
		}
		x_data[iter] = float(iter);
		y_data1[iter] = float(hawks);
		y_data2[iter] = float(doves);
	}
}
/*
this is not actually even close to real chicken, as its way simpler
the real one is not iterated, there are no meet chances, no resources, etc
this was basically me getting bored and merging chicken with some evolutionary mess
*/
void chicken::menu()
{
	for (auto& item : matrix.items)
		ImGui::SliderInt((item.name + "s").c_str(), &item.count, 0, 100);
	
	ImGui::SliderInt("Iterations", &iterations, 1, 100);
	ImGui::SliderFloat("Meet chance", &meet_chance, 0.f, 100.f, "%.1f%%");
	ImGui::SliderInt("Starting resources", &start_res, 0, 50);

	matrix.show_matrix(40.f);

	ImGui::Plot("chicken", conf);
	if (ImGui::Button("Run"))
		simulation();
}
