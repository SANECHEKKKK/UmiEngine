module;
#include <EngineApi/EngineApi.h>
#include <string>
#include <filesystem>

export module LevelManager;

import Registry;
import TextureManager;
import ModelManager;
import ScriptManager;

export namespace Umi
{
    struct CurrentLevel
    {
        std::string levelName;
        std::filesystem::path levelPath;
    };
    
    class ENGINE_API LevelManager
    {
    private:
        Registry& registry;
        TextureManager& textureManager;
        ModelManager& modelManager;
        ScriptManager& scriptManager;

    public:
        CurrentLevel currentLevel;
        
        // Saves to the current level path
        bool SaveLevel();
        //Saves to the selected path
        bool SaveLevel(const std::string& path);
        
        bool LoadLevel(const std::string& path);
        bool CreateLevel(const std::string& path);

        LevelManager(Registry& registry, TextureManager& textureManager, ModelManager& modelManager, ScriptManager& scriptManager) : registry(registry), textureManager(textureManager), modelManager(modelManager), scriptManager(scriptManager) {};
    };
}