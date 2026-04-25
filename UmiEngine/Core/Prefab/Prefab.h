#pragma once
#include <optional>
#include <string>
#include <functional>
#include <Registry/Registry.h>

namespace Umi
{
	using PrefabName = std::string;

	struct PrefabComponent 
	{
		std::type_index type;
		std::function<void(Registry&, Entity)> apply;
	};

	struct Prefab
	{
		std::vector<PrefabComponent> components;

		Prefab() = default;
		~Prefab() = default;
	};
}