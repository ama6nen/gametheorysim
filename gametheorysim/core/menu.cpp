#include "menu.hpp"
#include <tchar.h>
#include "../games/chicken.hpp"
#include "../games/ipd.hpp"

#pragma comment(lib,"d3d11.lib")


void CreateRenderTarget()
{
	ID3D11Texture2D* pBackBuffer;
	menu::g_swapchain->GetBuffer(0, IID_PPV_ARGS(&pBackBuffer));
	menu::g_d3d_device->CreateRenderTargetView(pBackBuffer, NULL, &menu::g_render_view);
	pBackBuffer->Release();
}
void CleanupRenderTarget()
{
	if (menu::g_render_view) { menu::g_render_view->Release(); menu::g_render_view = NULL; }
}
bool CreateDeviceD3D(HWND hWnd)
{
	DXGI_SWAP_CHAIN_DESC sd;
	ZeroMemory(&sd, sizeof(sd));
	sd.BufferCount = 2;
	sd.BufferDesc.Width = 0;
	sd.BufferDesc.Height = 0;
	sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	sd.BufferDesc.RefreshRate.Numerator = 60;
	sd.BufferDesc.RefreshRate.Denominator = 1;
	sd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
	sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	sd.OutputWindow = hWnd;
	sd.SampleDesc.Count = 1;
	sd.SampleDesc.Quality = 0;
	sd.Windowed = TRUE;
	sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

	UINT createDeviceFlags = 0;
	D3D_FEATURE_LEVEL featureLevel;
	const D3D_FEATURE_LEVEL featureLevelArray[2] = { D3D_FEATURE_LEVEL_11_0, D3D_FEATURE_LEVEL_10_0, };
	if (D3D11CreateDeviceAndSwapChain(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, createDeviceFlags, featureLevelArray, 2, D3D11_SDK_VERSION, &sd, &menu::g_swapchain, &menu::g_d3d_device, &featureLevel, &menu::g_d3d_device_context) != S_OK)
		return false;

	CreateRenderTarget();
	return true;
}
void CleanupDeviceD3D()
{
	CleanupRenderTarget();
	if (menu::g_swapchain) { menu::g_swapchain->Release(); menu::g_swapchain = NULL; }
	if (menu::g_d3d_device_context) { menu::g_d3d_device_context->Release(); menu::g_d3d_device_context = NULL; }
	if (menu::g_d3d_device) { menu::g_d3d_device->Release(); menu::g_d3d_device = NULL; }
}

void menu::setup()
{
	g_wc = { sizeof(WNDCLASSEX), CS_CLASSDC, WndProc, 0L, 0L, GetModuleHandle(NULL), NULL, NULL, NULL, NULL, _T("Game theory simulation"), NULL };
	RegisterClassEx(&g_wc);
	g_hwnd = CreateWindow(g_wc.lpszClassName, _T("Game theory simulation"), WS_OVERLAPPEDWINDOW, 100, 100, 1000, 700, NULL, NULL, g_wc.hInstance, NULL);

	if (!CreateDeviceD3D(g_hwnd))
	{
		CleanupDeviceD3D();
		UnregisterClass(g_wc.lpszClassName, g_wc.hInstance);
		return;
	}

	ShowWindow(g_hwnd, SW_SHOWDEFAULT);
	UpdateWindow(g_hwnd);

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
	
	(void)io;
	ImGui::StyleColorsDark();
	auto& style = ImGui::GetStyle();
	style.FramePadding.x = 0.f;
	ImGui_ImplWin32_Init(g_hwnd);
	ImGui_ImplDX11_Init(menu::g_d3d_device, menu::g_d3d_device_context);

	chicken::setup();
	ipd::setup();
}
void menu::run()
{
	static ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();
	
	if (ImGui::Begin("Main window", 0, ImGuiWindowFlags_NoCollapse))
	{	

		if (ImGui::BeginTabBar("##tabs", ImGuiTabBarFlags_None))
		{
			if (ImGui::BeginTabItem("Chicken")) {
				chicken::menu();
				ImGui::EndTabItem();
			}
				
			if (ImGui::BeginTabItem("Iterated PD")) {
				ipd::menu();
				ImGui::EndTabItem();
			}
				
		}
		ImGui::EndTabBar();
		ImGui::End();
	}	
	
	ImGui::Render();
	menu::g_d3d_device_context->OMSetRenderTargets(1, &menu::g_render_view, NULL);
	menu::g_d3d_device_context->ClearRenderTargetView(menu::g_render_view, (float*)&clear_color);
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

	menu::g_swapchain->Present(1, 0);
}

void menu::end()
{
	ImGui_ImplDX11_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();

	CleanupDeviceD3D();
	DestroyWindow(g_hwnd);
	UnregisterClass(g_wc.lpszClassName, g_wc.hInstance);
}

LRESULT __stdcall menu::WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	if (ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam))
		return true;

	switch (msg)
	{
	case WM_SIZE:
		if (menu::g_d3d_device != NULL && wParam != SIZE_MINIMIZED)
		{
			CleanupRenderTarget();
			menu::g_swapchain->ResizeBuffers(0, (UINT)LOWORD(lParam), (UINT)HIWORD(lParam), DXGI_FORMAT_UNKNOWN, 0);
			CreateRenderTarget();
		}
		return 0;
	case WM_SYSCOMMAND:
		if ((wParam & 0xfff0) == SC_KEYMENU)
			return 0;
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	}
	return DefWindowProc(hWnd, msg, wParam, lParam);
}
