module;
#include <memory>
#include <functional>
#include <unordered_map>
#include <string_view>
#include <type_traits>

#include <EngineApi/EngineApi.h>
export module ScriptManager;
#include <string>

import EngineContext;
import BasicScript;
import Script;

export namespace Umi
{
    class ENGINE_API ScriptManager
    {
    private:
        EngineContext& engineContext;

        struct ScriptType
        {
            std::function<std::unique_ptr<BasicScript>()> create;
        };
        std::unordered_map<std::string, ScriptType> types;

    public:

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
            return it == types.end() ? nullptr : it->second.create();
        }

        void Update()
        {
			for (auto e : engineContext.registry.View<Scripts>())
			{
				auto& script = engineContext.registry.GetComponent<Scripts>(e);
				for (auto& s : script.scripts)
				{
					s->Update();
				}
			}
        }

        ScriptManager(EngineContext& engineContext) : engineContext(engineContext) {}
    };
}