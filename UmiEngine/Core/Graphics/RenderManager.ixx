module;
#include <Graphics/d3dx12.h>
export module RenderManager;


import EngineContext;
import GraphicsContext;

export namespace Umi
{
	class RenderManager
	{
	private:
		EngineContext& engineContext;
		

	public:
		void Render();

		RenderManager(EngineContext& context, GraphicsContext& graphicsContext);
		~RenderManager() = default;
	};
}