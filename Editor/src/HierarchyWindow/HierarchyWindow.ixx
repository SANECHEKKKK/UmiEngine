export module HierarchyWindow;

import Registry;
import EditorContext;

import Entity;

export namespace Umi
{
	class HierarchyWindow
	{
	private:
		EditorContext& editorContext;
		Registry& registry;
		void CreateEntity();

		Entity entityToRename{ INVALID_ENTITY };
		bool renamingActive = false;
		bool renamingJustStarted = false;
		char renameBuffer[128]{};

	public:
		void Draw();


		HierarchyWindow(Registry& registry, EditorContext& context);
	};
}