module;
#include <string>
#include <vector>
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

		struct PendingModelAssign { Entity entity; std::string path; };
		std::vector<PendingModelAssign> pendingAssigns;

	public:
		void Draw();
		void ProcessPending();

		InspectorWindow(EngineContext& engineContext, EditorContext& context);
	};
}