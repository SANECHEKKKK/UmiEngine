module;
#include <cstdint>
#include <EngineApi/EngineApi.h>
export module CameraManager;

import EngineContext;
import Registry;
import Camera;
import Transform;

export namespace Umi
{
    class ENGINE_API CameraManager
    {
    private:
        Camera editorCamera;
        Transform editorCameraTransform;

        Registry& registry;
        uint32_t viewportWidth = 1280; // nonzero defaults; overwritten at startup
        uint32_t viewportHeight = 720;

        bool useEditorCamera = true;

        void CameraController();

    public:
        void UseEditorCamera() { useEditorCamera = true; } // editor calls on Stop
        void UseGameCamera() { useEditorCamera = false; } // editor calls on Play

        void Update();
        void SetViewportSize(uint32_t width, uint32_t height);

        Camera* GetMainCamera();

        CameraManager(EngineContext& engineContext) : registry(engineContext.registry) {}
    };
}
