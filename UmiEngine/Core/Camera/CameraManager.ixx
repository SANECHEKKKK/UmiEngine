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

	public:
		void Update();

		Camera* GetMainCamera();

		CameraManager(EngineContext& engineContext) : registry(engineContext.registry) {}
	};
}