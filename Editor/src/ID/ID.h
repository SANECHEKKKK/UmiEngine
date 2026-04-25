#pragma once
#include <cstdint>
#include <string>

namespace Umi
{
	struct IDComponent
	{
		uint32_t id;
		std::string name = "DEFAULT_NAME";
	};
}