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

		void DrawID(Entity entity);
		void DrawTransform(Entity entity);
		void DrawModel(Entity entity);
		void DrawTexture(Entity entity);
		void DrawCamera(Entity entity);
		void DrawScript(Entity entity);
		void DrawColliderBox(Entity entity);
		void DrawText(Entity entity);
		void DrawAnimator(Entity entity);

		void DrawAddComponentButton(Entity entity);
		void DrawAddScriptButton(Entity entity);

	public:
		void Draw();
		void ProcessPending();
		

		InspectorWindow(EngineContext& engineContext, EditorContext& context);
	};
}