module;
//-----Needed for Constructor-----//
#include <windows.h>
#include <Graphics/d3dx12.h>
//--------------------------------//

//-----------ImGuiThings----------//
#include <ImGui/imgui.h>
#include <ImGui/imgui_impl_win32.h>
#include <ImGui/imgui_impl_dx12.h>
#include <ImGui/imgui_internal.h>
//--------------------------------//

#include <cstdint>
module ImGuiManager;

import GraphicsManager;
import EngineContext;
//import Registry;
//import EditorContext;

using namespace Umi;

void ImGuiManager::RenderDockingSpace()
{
    ImGuiViewport* viewport = ImGui::GetMainViewport();
    ImGui::SetNextWindowPos(viewport->Pos);
    ImGui::SetNextWindowSize(viewport->Size);
    ImGui::SetNextWindowViewport(viewport->ID);

    ImGuiWindowFlags dockspace_flags =
        ImGuiWindowFlags_NoTitleBar |
        ImGuiWindowFlags_NoCollapse |
        ImGuiWindowFlags_NoResize |
        ImGuiWindowFlags_NoMove |
        ImGuiWindowFlags_NoBringToFrontOnFocus |
        ImGuiWindowFlags_NoNavFocus |
        ImGuiWindowFlags_NoBackground |
        ImGuiWindowFlags_MenuBar;

    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));


    ImGui::Begin("DockSpace", nullptr, dockspace_flags);
    ImGui::PopStyleVar(3);

    ImGuiID dockspace_id = ImGui::GetID("MainDockSpace");
    ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), ImGuiDockNodeFlags_PassthruCentralNode);

    if (ImGui::DockBuilderGetNode(dockspace_id) == nullptr ||
        ImGui::DockBuilderGetNode(dockspace_id)->IsEmpty())
    {
        ImGui::DockBuilderRemoveNode(dockspace_id);
        ImGui::DockBuilderAddNode(dockspace_id, ImGuiDockNodeFlags_PassthruCentralNode | ImGuiDockNodeFlags_DockSpace);
        ImGui::DockBuilderSetNodeSize(dockspace_id, viewport->Size);

        ImGuiID main_id = dockspace_id;
        ImGuiID toolbar_id = ImGui::DockBuilderSplitNode(main_id, ImGuiDir_Up, 0.06f, nullptr, &main_id);

        ImGui::DockBuilderDockWindow("Toolbar", toolbar_id);
        // dock your other default panels into main_id / further splits here

        ImGui::DockBuilderFinish(dockspace_id);
    }
    
    ImGui::End();
}

void ImGuiManager::Render()
{
    ImGui_ImplDX12_NewFrame();
    ImGui_ImplWin32_NewFrame();
    ImGui::NewFrame();

    RenderDockingSpace();

#ifdef _DEBUG
    ImGui::Begin("Debug");
    ImGui::Text("FPS: %.1f", ImGui::GetIO().Framerate);
    ImGui::End();
#endif //_DEBUG

    // ImGui::ShowDemoWindow();
    topBar.Draw();
    inspectorWindow.Draw();
    hierarchyWindow.Draw();
    assetBrowser.Draw();
    errorWindow.Draw();
    DrawViewportWindow();
    DrawToolbar();

    ImGui::Render();
    ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), commandList);

    ImGui::UpdatePlatformWindows();
    ImGui::RenderPlatformWindowsDefault();
}

void ImGuiManager::ProcessDeferred()
{
    assetBrowser.ProcessPending();
    inspectorWindow.ProcessPending();
}

void ImGuiManager::DrawViewportWindow()
{
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
    ImGui::Begin("Viewport");

    ImVec2 avail = ImGui::GetContentRegionAvail();
    uint32_t w = (uint32_t)(avail.x > 1.0f ? avail.x : 1.0f);
    uint32_t h = (uint32_t)(avail.y > 1.0f ? avail.y : 1.0f);
    graphics.RequestViewportResize(w, h);

    ImGui::Image((ImTextureID)graphics.GetViewportTextureHandle().ptr, avail);

    ImGui::End();
    ImGui::PopStyleVar();
}

void ImGuiManager::DrawToolbar()
{
    ImGuiWindowClass toolbarClass;
    toolbarClass.DockNodeFlagsOverrideSet =
        ImGuiDockNodeFlags_NoTabBar | ImGuiDockNodeFlags_NoDockingSplit;
    ImGui::SetNextWindowClass(&toolbarClass);

    ImGuiWindowFlags flags =
        ImGuiWindowFlags_NoTitleBar |   // still useful for the floating case
        ImGuiWindowFlags_NoCollapse |
        ImGuiWindowFlags_NoResize |
        ImGuiWindowFlags_NoMove |
        ImGuiWindowFlags_NoBringToFrontOnFocus |
        ImGuiWindowFlags_NoNavFocus |
        ImGuiWindowFlags_NoScrollbar;

    ImGui::Begin("Toolbar", nullptr, flags);
    bool playing = editorContext.playState != PlayState::Edit;


    ImVec2 button_size = ImVec2(0, 0);

    float button_width = button_size.x;
    if (button_width <= 0.0f)
    {
        button_width = ImGui::CalcTextSize("Play").x + ImGui::GetStyle().FramePadding.x * 2.0f;
    }

    float available_width = ImGui::GetContentRegionAvail().x;

    float off_x = (available_width - button_width) * 0.5f;
    if (off_x > 0.0f)
    {
        ImGui::SetCursorPosX(ImGui::GetCursorPosX() + off_x);
    }


    if (!playing)
    {
        if (ImGui::Button("Play", button_size)) editorContext.requestPlay = true;
    }
    else
    {
        button_width = (ImGui::CalcTextSize("Stop").x + ImGui::CalcTextSize("Pause").x) - ImGui::GetStyle().FramePadding
            .x * 2.0f;


        // off_x -= ImGui::CalcTextSize("Pause").x;
        if (off_x > 0.0f)
        {
            ImGui::SetCursorPosX(ImGui::GetCursorPosX() - ImGui::CalcTextSize("Pause").x);
        }


        if (ImGui::Button("Stop", button_size)) editorContext.requestStop = true;
        ImGui::SameLine();
        bool paused = editorContext.playState == PlayState::Paused;
        if (ImGui::Button(paused ? "Resume" : "Pause", button_size)) editorContext.requestPauseToggle = true;
        ImGui::SameLine();
        ImGui::TextDisabled(paused ? "(paused)" : "(playing)");
    }
    ImGui::End();
}

ImGuiManager::ImGuiManager(HWND hwnd, ImguiInitInfo* initInfo, EngineContext& engineContext,
                           EditorContext& editorContext, GraphicsManager& graphics)
    : engineContext(engineContext), editorContext(editorContext), graphics(graphics)
{
    commandList = initInfo->commandList;

    ImGui_ImplWin32_EnableDpiAwareness();
    //float main_scale = ImGui_ImplWin32_GetDpiScaleForMonitor(::MonitorFromPoint(POINT{ 0, 0 }, MONITOR_DEFAULTTOPRIMARY));


    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard; // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad; // Enable Gamepad Controls
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable; // Enable Docking
    //io.ConfigViewportsNoAutoMerge = true;
    //io.ConfigViewportsNoTaskBarIcon = true;

    ImGui::StyleColorsDark();
    //ImGui::StyleColorsLight();

    // Setup scaling
    ImGuiStyle& style = ImGui::GetStyle();
    //style.ScaleAllSizes(main_scale);        // Bake a fixed style scale. (until we have a solution for dynamic style scaling, changing this requires resetting Style + calling this again)
    //style.FontScaleDpi = main_scale;        // Set initial font scale. (in docking branch: using io.ConfigDpiScaleFonts=true automatically overrides this for every window depending on the current monitor)
    //io.ConfigDpiScaleFonts = true;          // [Experimental] Automatically overwrite style.FontScaleDpi in Begin() when Monitor DPI changes. This will scale fonts but _NOT_ scale sizes/padding for now.
    //io.ConfigDpiScaleViewports = true;      // [Experimental] Scale Dear ImGui and Platform Windows when Monitor DPI changes.

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
    initStruct.SrvDescriptorAllocFn = [](ImGui_ImplDX12_InitInfo* info, D3D12_CPU_DESCRIPTOR_HANDLE* cpu,
                                         D3D12_GPU_DESCRIPTOR_HANDLE* gpu)
    {
        static_cast<DescriptorHeapAllocator*>(info->UserData)->Alloc(cpu, gpu);
    };
    initStruct.SrvDescriptorFreeFn = [](ImGui_ImplDX12_InitInfo* info, D3D12_CPU_DESCRIPTOR_HANDLE cpu,
                                        D3D12_GPU_DESCRIPTOR_HANDLE gpu)
    {
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
