
//-----Needed for Constructor-----//
#include <windows.h>
#include <d3d11.h>
#include <Registry/Registry.h>
//--------------------------------//

//-----------ImGuiThings----------//
#include <ImGui/ImGuiManager.h>
#include <ImGui/imgui.h>
#include <ImGui/backends/imgui_impl_win32.h>
#include <ImGui/backends/imgui_impl_dx11.h>
//--------------------------------//

#include <ID/ID.h>



void Umi::ImGuiManager::InspectorWindow()
{

}

void Umi::ImGuiManager::DrawBegin()
{
	ImGui_ImplWin32_NewFrame();
	ImGui_ImplDX11_NewFrame();
	ImGui::NewFrame();

	InspectorWindow();
}

void Umi::ImGuiManager::DrawEnd()
{
	hierarchyWindow.Draw();
	inspectorWindow.Draw();

	ImGui::Render();
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
}

Umi::ImGuiManager::ImGuiManager(HWND hwnd, ID3D11Device* device, ID3D11DeviceContext* deviceContext, Registry& registry, EditorContext& editorContext)
	: registry(registry), editorContext(editorContext), hierarchyWindow(registry, editorContext), inspectorWindow(registry, editorContext)
{
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	(void)io;

	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
	//io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;

	ImGui::StyleColorsDark();

	ImGui_ImplWin32_Init(hwnd);
	ImGui_ImplDX11_Init(device, deviceContext);
}

Umi::ImGuiManager::~ImGuiManager()
{
	ImGui_ImplDX11_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();
}
