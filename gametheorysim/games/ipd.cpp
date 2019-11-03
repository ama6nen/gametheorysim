#include "ipd.hpp"

namespace ipd {
	pfmatrix matrix = pfmatrix();
	int iterations = 150;
	ImGui::PlotConfig conf;

	constexpr size_t buf_size = 150;
	static float x_data[buf_size];
	static float y_data1[buf_size];
	static float y_data2[buf_size];
	int left = 0;
	int top = 0;
}
void gen() {
	for (size_t i = 0; i < ipd::buf_size; ++i) {
		float f = float(i);
		ipd::x_data[i] = f;
		ipd::y_data1[i] = f;
		ipd::y_data2[i] = f;
	}
}

void ipd::setup()
{
	matrix.items = { { "Defect", 1 }, { "Cooperate", 1 } };
	std::vector<payoff> one = {};
	one.push_back({ -2, -2 });
	one.push_back({ 3, -1 });
	matrix.payoffs.push_back(one);
	std::vector<payoff> two = {};
	two.push_back({ -1, 3 });
	two.push_back({ 1, 1 });
	matrix.payoffs.push_back(two);
	gen();

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

void ipd::simulation()
{
	int left_score = 0, top_score = 0;
	int max_score = INT_MIN;
	int min_score = INT_MAX;
	std::vector<bool> ldmem{}; //left defect memory
	std::vector<bool> tdmem{}; //top defect memory

	for (int iter = 1; iter < iterations; iter++) {

		bool lcoop = true;
		bool tcoop = true;
		switch (left) {
		case 0: //random
			lcoop = utils::random(0, 1) == 1;
			break;
		case 1: // always defect
			lcoop = false;
			break;
		case 2: // always coop
			lcoop = true;
			break;

		case 3: //tit for tat
			if (iter == 1)
				lcoop = true;	
			else {
				lcoop = !tdmem.at(iter - 2);
			}
			break;
		case 4: //forg. tit for tat
			if (iter == 1 || iter == 2)
				lcoop = true;
			else {
				lcoop = !(tdmem.at(iter - 2) && tdmem.at(iter - 3));
			}
			break;
		case 5: //grudger
			if (iter == 1 )
				lcoop = true;
			else {
				for (auto def : tdmem)
					if (def)
						lcoop = false;
			}
			break;
		case 6: //prober
			if (iter == 1)
				lcoop = false;
			else if (iter == 2 || iter == 3)
				lcoop = true;
			else {
				lcoop = !(!tdmem.at(1) && !tdmem.at(2));
			}
			break;
		case 7: //soft majo
		{
			int defects = 0;
			int coops = 0;
			for (auto def : tdmem)
			{
				if (def)
					defects++;
				else
					coops++;
			}
			lcoop = (coops >= defects);
		}	break;
		case 8: //hard majo
		{
			int defects = 0;
			int coops = 0;
			for (auto def : tdmem)
			{
				if (def)
					defects++;
				else
					coops++;
			}
			lcoop = !(defects >= coops);
		}	break;
		case 9: //ddc
		{
			lcoop = (iter % 3 == 0);
		}	break;
		case 10: //ccd
		{
			lcoop = !(iter % 3 == 0);
		}	break;
		case 11: //susp. tit for tat
			if (iter == 1)
				lcoop = false;
			else {
				lcoop = !tdmem.at(iter - 2);
			}
			break;
		default:break;
		}
		switch (top) {
		case 0: //random
			tcoop = utils::random(0, 1) == 1;
			break;
		case 1: // always defect
			tcoop = false;
			break;
		case 2: // always coop
			tcoop = true;
			break;
		case 3: //tit for tat
			if (iter == 1)
				tcoop = true;
			else {
				tcoop = !ldmem.at(iter - 2);
			}
			break;
		case 4: //forg. tit for tat
			if (iter == 1 || iter == 2)
				tcoop = true;
			else {
				tcoop = !(ldmem.at(iter - 2) && ldmem.at(iter - 3));
			}
			break;
		case 5: //grudger
			if (iter == 1)
				tcoop = true;
			else {
				for (auto def : ldmem)
					if (def)
						tcoop = false;
			}
			break;
		case 6: //prober
			if (iter == 1)
				tcoop = false;
			else if (iter == 2 || iter == 3)
				tcoop = true;
			else {
				tcoop = !(!ldmem.at(1) && !ldmem.at(2));
			}
			break;
		case 7: //soft majo
		{
			int defects = 0;
			int coops = 0;
			for (auto def : ldmem)
			{
				if (def)
					defects++;
				else
					coops++;
			}
			tcoop = (coops >= defects);
		}	break;
		case 8: //hard majo
		{
			int defects = 0;
			int coops = 0;
			for (auto def : ldmem)
			{
				if (def)
					defects++;
				else
					coops++;
			}
			tcoop = !(defects >= coops);
		}	break;
		case 9: //ddc
		{
			tcoop = (iter % 3 == 0);
		}	break;
		case 10: //ccd
		{
			tcoop = !(iter % 3 == 0);
		}	break;
		case 11: //susp. tit for tat
			if (iter == 1)
				tcoop = false;
			else {
				tcoop = !tdmem.at(iter - 2);
			}
			break;
		default:break;
		}

		ldmem.push_back(!lcoop);
		tdmem.push_back(!tcoop);
		auto payoff = matrix.payoffs[lcoop][tcoop];
		left_score += payoff.l;
		top_score += payoff.r;
		x_data[iter] = float(iter);
		y_data1[iter] = float(left_score);
		y_data2[iter] = float(top_score);

		if (left_score > max_score)
			max_score = left_score;
		if (top_score > max_score)
			max_score = top_score;

		if (left_score < min_score)
			min_score = left_score;
		if (top_score < min_score)
			min_score = top_score;
	}
	conf.scale.max = max_score;
	conf.scale.min = min_score;
}

void ipd::menu()
{
	ImGui::TextColored(ImVec4(0,1,0,1), "Green: left player");
	ImGui::TextColored(ImVec4(1, 0, 0, 1), "Red: top player");

	ImGui::SliderInt("Iterations", &iterations, 10, 150);
	const static char* items = "random\0always defect\0always cooperate\0tit fot tat\0forgiving tit for tat\0grudger/spiteful\0prober\0soft_majo\0hard_majo\0ddc\0ccd";
	ImGui::Combo("Left player strategy", &left, items);
	ImGui::Combo("Top player strategy", &top, items);
	matrix.show_matrix(65.f);
	ImGui::Plot("ipd", conf);
	if (ImGui::Button("Run"))
		simulation();
}
