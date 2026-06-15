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

		struct PendingAssign { Entity entity; std::string path; };
		std::vector<PendingAssign> pendingModelAssigns;
		std::vector<PendingAssign> pendingTextureAssigns;

	public:
		void Draw();
		void ProcessPending();

		InspectorWindow(EngineContext& engineContext, EditorContext& context);
	};
}