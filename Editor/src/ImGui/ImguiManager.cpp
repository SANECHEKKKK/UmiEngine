module;
//-----Needed for Constructor-----//
#include <windows.h>
#include <Graphics/d3dx12.h>
//--------------------------------//

//-----------ImGuiThings----------//
#include <ImGui/imgui.h>
#include <ImGui/imgui_impl_win32.h>
#include <ImGui/imgui_impl_dx12.h>
//--------------------------------//
module ImGuiManager;

import GraphicsManager;
//import Registry;
//import EditorContext;

using namespace Umi;

void ImGuiManager::Render()
{
	//ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), commandList);

	ImGui_ImplDX12_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();

	//ImGui::ShowDemoWindow();

	ImGui::Begin("Debug");

	ImGui::Text("Hello from ImGui!");
	ImGui::Text("FPS: %.1f", ImGui::GetIO().Framerate);

	ImGui::End();

	inspectorWindow.Draw();
	hierarchyWindow.Draw();

	ImGui::Render();
	ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), commandList);

	ImGui::UpdatePlatformWindows();
	ImGui::RenderPlatformWindowsDefault();
}

ImGuiManager::ImGuiManager(HWND hwnd, ImguiInitInfo* initInfo, Registry& registry, EditorContext& editorContext) : registry(registry), editorContext(editorContext)
{
	commandList = initInfo->commandList;
	
	//ImGui_ImplWin32_EnableDpiAwareness();
	//float main_scale = ImGui_ImplWin32_GetDpiScaleForMonitor(::MonitorFromPoint(POINT{ 0, 0 }, MONITOR_DEFAULTTOPRIMARY));


	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;         // Enable Docking
	io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;       // Enable Multi-Viewport / Platform Windows
	io.ConfigViewportsNoAutoMerge = true;
	io.ConfigViewportsNoTaskBarIcon = true;

	ImGui::StyleColorsDark();
	//ImGui::StyleColorsLight();

	// Setup scaling
	ImGuiStyle& style = ImGui::GetStyle();
	//style.ScaleAllSizes(main_scale);        // Bake a fixed style scale. (until we have a solution for dynamic style scaling, changing this requires resetting Style + calling this again)
	//style.FontScaleDpi = main_scale;        // Set initial font scale. (in docking branch: using io.ConfigDpiScaleFonts=true automatically overrides this for every window depending on the current monitor)
	io.ConfigDpiScaleFonts = true;          // [Experimental] Automatically overwrite style.FontScaleDpi in Begin() when Monitor DPI changes. This will scale fonts but _NOT_ scale sizes/padding for now.
	io.ConfigDpiScaleViewports = true;      // [Experimental] Scale Dear ImGui and Platform Windows when Monitor DPI changes.

	// When viewports are enabled we tweak WindowRounding/WindowBg so platform windows can look identical to regular ones.
	if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
	{
		style.WindowRounding = 0.0f;
		style.Colors[ImGuiCol_WindowBg].w = 1.0f;
	}

	ImGui_ImplWin32_Init(hwnd);

	ImGui_ImplDX12_InitInfo initStruct{};
	initStruct.Device = initInfo->device;
	initStruct.CommandQueue = initInfo->commandQueue;
	initStruct.NumFramesInFlight = 2;
	initStruct.RTVFormat = DXGI_FORMAT_R8G8B8A8_UNORM;
	initStruct.DSVFormat = DXGI_FORMAT_UNKNOWN;
	initStruct.SrvDescriptorHeap = initInfo->imguiSRVDescriptorHeap;
	initStruct.UserData = &initInfo->imguiSrvAllocator;
	initStruct.SrvDescriptorAllocFn = [](ImGui_ImplDX12_InitInfo* info, D3D12_CPU_DESCRIPTOR_HANDLE* cpu, D3D12_GPU_DESCRIPTOR_HANDLE* gpu) {
		static_cast<DescriptorHeapAllocator*>(info->UserData)->Alloc(cpu, gpu);
		};
	initStruct.SrvDescriptorFreeFn = [](ImGui_ImplDX12_InitInfo* info, D3D12_CPU_DESCRIPTOR_HANDLE cpu, D3D12_GPU_DESCRIPTOR_HANDLE gpu) {
		static_cast<DescriptorHeapAllocator*>(info->UserData)->Free(cpu, gpu);
		};

	ImGui_ImplDX12_Init(&initStruct);
}

ImGuiManager::~ImGuiManager()
{
	ImGui_ImplDX12_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();
}