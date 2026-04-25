#pragma once
#include <windows.h>
#include <d3d11.h>
#include <HierarchyWindow/HierarchyWindow.h>
#include <InspectorWindow/InspectorWindow.h>

namespace Umi
{
	class Registry;
	struct EditorContext;

	class ImGuiManager
	{
	private:
		Registry& registry;
		EditorContext& editorContext;
		HierarchyWindow hierarchyWindow{ registry, editorContext };
		InspectorWindow inspectorWindow{ registry, editorContext };

		void InspectorWindow();
	public:
		void DrawBegin();
		void DrawEnd();

		ImGuiManager(HWND hwnd, ID3D11Device* device, ID3D11DeviceContext* deviceContext, Registry& registry, EditorContext& editorContext);
		~ImGuiManager();
	};
}