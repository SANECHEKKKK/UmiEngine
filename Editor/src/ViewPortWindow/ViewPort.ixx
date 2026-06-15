export module ViewPort;

import Registry;
import EditorContext;

export namespace Umi
{
	class ViewPortWindow
	{
	private:
		EditorContext& editorContext;
		Registry& registry;

	public:
		void Draw();

		ViewPortWindow(Registry& registry, EditorContext& context);
	};
}