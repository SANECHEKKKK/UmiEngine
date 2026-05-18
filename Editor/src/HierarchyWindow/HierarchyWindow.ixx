export module HierarchyWindow;

import Registry;
import EditorContext;

export namespace Umi
{
	class HierarchyWindow
	{
	private:
		EditorContext& editorContext;
		Registry& registry;
		void CreateEntity();

	public:
		void Draw();


		HierarchyWindow(Registry& registry, EditorContext& context);
	};
}