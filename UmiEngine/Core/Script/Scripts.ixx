module;
#include <vector>
#include <memory>
#include <string>
#include <EngineApi/EngineApi.h>
export module Script;

import BasicScript;

export namespace Umi
{
	struct ENGINE_API ScriptInstance {
		std::string name;
		std::unique_ptr<BasicScript> instance;
	};
	
	struct ENGINE_API Scripts
	{
		std::vector<ScriptInstance> scripts;

		Scripts() = default;

		Scripts(const Scripts&) = delete;
		Scripts& operator=(const Scripts&) = delete;

		Scripts(Scripts&&) = default;
		Scripts& operator=(Scripts&&) = default;
	};
}