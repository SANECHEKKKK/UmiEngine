module;
#include <windows.h>
#include <Graphics/d3dx12.h>
export module ImGuiManager;

import Registry;
import EditorContext;
import EngineContext;     // NEW

import InspectorWindow;
import GraphicsManager;
import HierarchyWindow;
import AssetBrowser;
import TopBar;

import Entity;

export namespace Umi
{
	class ImGuiManager
	{
    private:
        EngineContext& engineContext;
        EditorContext& editorContext;
        GraphicsManager& graphics;

        HierarchyWindow  hierarchyWindow{ engineContext.registry, editorContext };
        InspectorWindow  inspectorWindow{ engineContext, editorContext };
        AssetBrowser     assetBrowser{ engineContext, editorContext };
	    TopBar           topBar{ engineContext, editorContext };

        ID3D12GraphicsCommandList* commandList = nullptr;

        void RenderDockingSpace();
        void DrawViewportWindow();
		void DrawToolbar();
	    
    public:
        void Render();
        void ProcessDeferred();

        ImGuiManager(HWND hwnd, ImguiInitInfo* initInfo, EngineContext& engineContext,
            EditorContext& editorContext, GraphicsManager& graphics);
        ~ImGuiManager();
	};
}