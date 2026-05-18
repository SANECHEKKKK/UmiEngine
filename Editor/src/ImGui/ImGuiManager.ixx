module;
#include <windows.h>
#include <Graphics/d3dx12.h>
export module ImGuiManager;

import Registry;
import EditorContext;

import InspectorWindow;

import GraphicsManager;
import HierarchyWindow;

export namespace Umi
{
	class ImGuiManager
	{
	private:
		Registry& registry;
		EditorContext& editorContext;

		HierarchyWindow hierarchyWindow{ registry, editorContext };
		InspectorWindow inspectorWindow{ registry, editorContext };

		ID3D12GraphicsCommandList* commandList = nullptr;

		void InspectorWindow();
	public:
		void Render();

		//ImGuiManager(HWND hwnd, ImguiInitInfo* initInfo);
		ImGuiManager(HWND hwnd, ImguiInitInfo* initInfo, Registry& registry, EditorContext& editorContext);
		~ImGuiManager();
	};
}