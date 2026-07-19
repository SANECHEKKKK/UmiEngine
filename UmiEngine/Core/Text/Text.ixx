module;
#include <string>
#include <EngineApi/EngineApi.h>
export module Text;

import Math;

export namespace Umi
{
	struct ENGINE_API Text
	{
		std::string text;
		float4 color;
	};
}