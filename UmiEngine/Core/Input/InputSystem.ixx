module;
#include <EngineApi/EngineApi.h>
export module InputSystem;

import Math;
import Keyboard;

export namespace Umi
{
    class ENGINE_API InputSystem
    {
    private:

    public:
        Vector3 inputVelocity{};
        
        void Update();
    };
}
