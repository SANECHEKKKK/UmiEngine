module;
#include <memory>
#include <functional>
#include <unordered_map>
#include <string_view>
#include <type_traits>
#include <Windows.h>

#include <EngineApi/EngineApi.h>
export module ScriptManager;
#include <string>

import Registry;
import Script;
import Entity;
import CollisionManager;
import EventManager;

export namespace Umi
{
    class ENGINE_API ScriptManager
    {
    private:
        Registry& registry;
        CollisionManager& collisionManager;
        EventManager& eventManager;

        HMODULE gameModule = nullptr;

        struct ScriptType
        {
            std::function<std::unique_ptr<BasicScript>()> create;
        };
        std::unordered_map<std::string, ScriptType> types;

    public:
        std::vector<std::string> GetRegisteredNames() const
        {
            std::vector<std::string> out;
            out.reserve(types.size());
            for (auto& [name, type] : types) out.push_back(name);
            return out;
        }

        template<typename T>
        void RegisterScript(std::string_view name)
        {
            static_assert(std::is_base_of_v<BasicScript, T>,
                "T must derive from BasicScript");
            types[std::string(name)] = ScriptType{
                []() -> std::unique_ptr<BasicScript> { return std::make_unique<T>(); }
            };
        }

        std::unique_ptr<BasicScript> CreateScript(std::string_view name)
        {
            auto it = types.find(std::string(name));
            if (it == types.end()) return nullptr;
            auto s = it->second.create();
            return s;
        }

        void Update()
        {
			for (auto e : registry.View<Scripts>())
			{
				auto& script = registry.GetComponent<Scripts>(e);
				for (auto& s : script.scripts)
				{
					s.instance->Update();
				}
			}
        }

        void LoadGameScripts(const std::wstring& dllPath = L"Game.dll")
        {
            // if reloading, drop the old one first
            if (gameModule)
            {
                FreeLibrary(gameModule);
                gameModule = nullptr;
            }

            types.clear();   // wipe old registrations before re-registering

            gameModule = LoadLibraryW(dllPath.c_str());
            if (!gameModule)
                return;       // Game.dll not found next to the exe

            using RegisterFn = void(*)(ScriptManager&);
            auto registerFn = reinterpret_cast<RegisterFn>(
                GetProcAddress(gameModule, "RegisterScripts"));

            if (registerFn)
                registerFn(*this);   // this calls RegisterAllScripts inside the DLL
        }

		void AddScriptToEntity(Entity e, std::string_view name)
		{
			auto script = CreateScript(name);
			if (!script) return;

			script->Bind(e, &registry, &collisionManager, &eventManager);

			if (!registry.HasComponent<Scripts>(e))
				registry.AddComponent<Scripts>(e, Scripts{});

			registry.GetComponent<Scripts>(e).scripts.push_back({ std::string(name), std::move(script) });
		}

        ScriptManager(Registry& registry, CollisionManager& collisionManager, EventManager& eventManager) : registry(registry), collisionManager(collisionManager), eventManager(eventManager) {}
    };
}