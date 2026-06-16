module;
#include <EngineApi/EngineApi.h>
export module LevelManager;

import EngineContext;

export namespace Umi
{
    class ENGINE_API LevelManager
    {
    private:
        EngineContext& engineContext;
        
    public:
        bool SaveLevel();
        bool LoadLevel();
        
        LevelManager(EngineContext& engineContext) :  engineContext(engineContext) {};
    };
}
