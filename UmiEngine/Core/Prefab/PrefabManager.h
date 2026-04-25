#pragma once
#include <unordered_map>
#include <string>
#include <Prefab/Prefab.h>
#include <Entity/Entity.h>

namespace Umi
{
	struct EngineContext;

	class PrefabManager
	{
	private:
		std::unordered_map<PrefabName, Prefab> prefabs;
		EngineContext& engineContext;

	public:
		std::string ResolvePrefabPath(const std::string& folder, const std::string& name) const;

		void LoadPrefab(const std::string& folder, const PrefabName& prefabName);
		void ApplyPrefab(class Registry& registry, const PrefabName& prefabName, Entity e);
		
		const Prefab& GetPrefab(const PrefabName& prefabName) const;
		//size_t CreatePrefab(const Prefab& prefab);

		template<typename T>
		void AddPrefabComponent(Prefab& prefab, const T& component)
		{
			prefab.components.push_back({
				typeid(T),
				[component](Registry& registry, Entity e) {
					registry.AddComponent<T>(e, component);
				}
				});
		}

		bool CheckPrefabExists(const PrefabName& prefabName) const noexcept
		{
			return prefabs.find(prefabName) != prefabs.end();
		}

		PrefabManager(EngineContext& engineContext);
		~PrefabManager() = default;
	};
}