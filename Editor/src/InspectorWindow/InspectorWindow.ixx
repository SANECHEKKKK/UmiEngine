export module InspectorWindow;

import Registry;
import EditorContext;

export namespace Umi
{
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