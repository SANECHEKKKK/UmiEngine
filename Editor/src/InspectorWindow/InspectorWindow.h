#pragma once

namespace Umi
{
	class Registry;
	struct EditorContext;

	class InspectorWindow
	{
	private:
		EditorContext& editorContext;
		Registry& registry;

	public:
		void Draw();

		InspectorWindow(Registry& registry, EditorContext& context);
	};
}