module;
#include <vector>
#include <memory>
#include <EngineApi/EngineApi.h>
export module Script;

import BasicScript;

export namespace Umi
{
	struct ENGINE_API Script
	{
		std::vector<std::unique_ptr<BasicScript>> scripts;

		Script() = default;

		Script(const Script&) = delete;
		Script& operator=(const Script&) = delete;

		Script(Script&&) = default;
		Script& operator=(Script&&) = default;
	};
}