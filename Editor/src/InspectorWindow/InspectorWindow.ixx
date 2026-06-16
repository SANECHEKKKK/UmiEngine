module;
#include <string>
#include <vector>
#include <functional>
export module InspectorWindow;

import EngineContext;
import EditorContext;
import Entity;


export namespace Umi
{
	class InspectorWindow
	{
	private:
		EngineContext& engineContext;
		EditorContext& editorContext;

		struct PendingAssign { Entity entity; std::string path; };
		std::vector<PendingAssign> pendingModelAssigns;
		std::vector<PendingAssign> pendingTextureAssigns;

		std::vector<std::function<void()>> deferredActions;

	public:
		void Draw();
		void ProcessPending();
		

		InspectorWindow(EngineContext& engineContext, EditorContext& context);
	};
}