export module ErrorWindow;

import Registry;
import EditorContext;
import EngineContext;

import Entity;

export namespace Umi
{
	class ErrorWindow
	{
	private:
		EditorContext& editorContext;
		EngineContext& engineContext;

	public:
		void Draw();


		ErrorWindow(EngineContext& engineContext, EditorContext& context) : engineContext(engineContext), editorContext(context) {}
	};
}
