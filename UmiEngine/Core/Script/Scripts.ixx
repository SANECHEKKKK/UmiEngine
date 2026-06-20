module;
#include <vector>
#include <memory>
#include <string>
#include <EngineApi/EngineApi.h>
export module Script;

import BasicScript;

export namespace Umi
{
	// struct Script
	// {
	// 	BasicScript script;
	// 	std::string path;
	// };
	
	struct ENGINE_API Scripts
	{
		std::vector<std::unique_ptr<BasicScript>> scripts;

		Scripts() = default;

		Scripts(const Scripts&) = delete;
		Scripts& operator=(const Scripts&) = delete;

		Scripts(Scripts&&) = default;
		Scripts& operator=(Scripts&&) = default;
	};
}