module;
#include <string>
#include <EngineApi/EngineApi.h>
export module ID;

export namespace Umi
{
	struct ENGINE_API ID
	{
		std::string name = "DEFAULT_NAME";
		std::string tag = "DEFAULT_TAG";
	};
}