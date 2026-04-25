#pragma once
#include <Entity/Entity.h>

namespace Umi
{
	class Registry;
	struct EditorContext;

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