#pragma once
#include "pfmatrix.hpp"
#include "../games/chicken.hpp"


void pfmatrix::show_matrix(float align, int bounds)
{
	ImGui::BeginGroup(); //grouping not used for anything right now but i have it set up already in case i will in the future
	{
		//first row
		ImGui::Text("");
		ImGui::SameLine(align);
		int n = 1;
		for (auto item : items) {
			ImGui::Text(item.name.c_str());

			if(n != items.size())
				ImGui::SameLine(align * (n+1));
			n++;
		}
		n = 1;
		//all of the rest
		int x = 0;
		for (auto item : items) { 
			auto text = item.name.c_str();
			ImGui::Text(text);
			ImGui::SameLine(align);
			int y = 0;
			for (auto item2 : items) {
				auto text2 = item2.name.c_str();
				auto display = text[0] + std::string(" x ") + text2[0];
				if (ImGui::Button(display.c_str()))
					ImGui::OpenPopup(display.c_str());
				
				if (ImGui::BeginPopup(display.c_str()))
				{
					ImGui::PushItemWidth(100.f);
					auto left = text + std::string("(r)"); //r for row
					ImGui::SliderInt(left.c_str(), &payoffs[x][y].l, -bounds, bounds);
					auto right = text2 + std::string("(c)"); //c for column
					ImGui::SliderInt(right.c_str(), &payoffs[x][y].r, -bounds, bounds);
					ImGui::PopItemWidth();
					ImGui::EndPopup();
				}
				if (n != items.size())
					ImGui::SameLine(align * (n + 1));
				n++;
				y++;
			}
			x++;
			n = 1;
		}
	}
	ImGui::EndGroup();
}
