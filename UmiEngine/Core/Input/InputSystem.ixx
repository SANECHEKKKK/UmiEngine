module;
#include <EngineApi/EngineApi.h>
export module InputSystem;

import Math;
import Keyboard;

export namespace Umi
{
    class ENGINE_API InputSystem
    {
    protected:
        friend class Editor;
        void Update();

    public:
        inline static Vector3 inputVelocity;
        inline static float inputRotation;      //rotation in radians
    };
}
