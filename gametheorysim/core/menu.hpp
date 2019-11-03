#pragma once
#include "../imgui/imgui.h"
#include "../imgui/impl/imgui_impl_dx11.h"
#include "../imgui/impl/imgui_impl_win32.h"
#include <Windows.h>
#include <d3d11.h>

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

namespace menu {
	static ID3D11Device* g_d3d_device = NULL;
	static ID3D11DeviceContext* g_d3d_device_context = NULL;
	static IDXGISwapChain* g_swapchain = NULL;
	static ID3D11RenderTargetView* g_render_view = NULL;
	static HWND g_hwnd = NULL;
	static WNDCLASSEX g_wc;

	void setup();
	void run();
	void end();
	LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
}