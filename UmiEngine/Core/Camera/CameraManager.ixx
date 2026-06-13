module;
#include <cstdint>
export module CameraManager;

import EngineContext;
import Registry;
import Camera;

export namespace Umi
{
	class CameraManager
	{
	private:
		Registry& registry;
		uint32_t viewportWidth = 1280;   // nonzero defaults; overwritten at startup
		uint32_t viewportHeight = 720;

	public:
		void Update();
		void SetViewportSize(uint32_t width, uint32_t height);

		Camera* GetMainCamera();

		CameraManager(EngineContext& engineContext) : registry(engineContext.registry) {}
	};
}