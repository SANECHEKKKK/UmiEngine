module;
#include <unordered_map>
#include <memory>
#include <typeindex>
#include <EngineApi/EngineApi.h>
export module Registry;

import Entity;

namespace Umi
{

	template<typename... Components>
	class View;

	struct IComponentPool
	{
		virtual void Destroy(Entity e) = 0;
		virtual ~IComponentPool() = default;
	};

	template<typename T>
	class ComponentPool : public IComponentPool
	{
	public:
		std::unordered_map<Entity, T> components;
		std::vector<Entity> denseEntities;

		void Add(Entity e, const T& component)
		{
			components[e] = component;
			denseEntities.push_back(e);
		}

		void Add(Entity e, T&& component)
		{
			components[e] = std::move(component); // Move instead of copy!
			denseEntities.push_back(e);
		}

		T& Get(Entity e)
		{
			return components.at(e);
		}

		bool Has(Entity e) const
		{
			return components.contains(e);
		}

		void Destroy(Entity e) override
		{
			components.erase(e);

			auto it = std::find(denseEntities.begin(), denseEntities.end(), e);
			if (it != denseEntities.end())
			{
				denseEntities.erase(it);
			}
		}

		const std::vector<Entity>& GetEntities() const { return denseEntities; }
	};

	//-----------EXPORT-----------
	export class ENGINE_API Registry
	{
	private:
		std::vector<Entity> freeEntities;

		std::vector<std::unique_ptr<IComponentPool>> pools;
		std::unordered_map<std::type_index, IComponentPool*> poolMap;

	public:
		Entity maxEntity{ 0 };

		Entity CreateEntity()
		{
			if (!freeEntities.empty())
			{
				Entity e = freeEntities.back();
				freeEntities.pop_back();
				return e;
			}
			return maxEntity++;
		}

		void DestroyEntity(Entity e)
		{
			for (auto& pool : pools)
				pool->Destroy(e);

			freeEntities.push_back(e);
		}

		void Clear()
		{
			for (auto& pool : pools)
				pool.reset();
			freeEntities.clear();
			maxEntity = static_cast<Entity>(0);
		}
		
		template<typename T>
		ComponentPool<T>& GetPool()
		{
			std::type_index type = typeid(T);

			auto it = poolMap.find(type);
			if (it == poolMap.end())
			{
				auto pool = std::make_unique<ComponentPool<T>>();
				auto* raw = pool.get();

				pools.push_back(std::move(pool));
				poolMap[type] = raw;

				return *static_cast<ComponentPool<T>*>(raw);
			}

			return *static_cast<ComponentPool<T>*>(it->second);
		}

		template<typename T>
		void AddComponent(Entity e, const T& component)
		{
			GetPool<T>().Add(e, component);
		}

		template<typename T>
		void AddComponent(Entity e, T&& component)
		{
			GetPool<T>().Add(e, std::move(component));
		}

		template<typename T>
		bool HasComponent(Entity e)
		{
			return GetPool<T>().Has(e);
		}

		template<typename T>
		T& GetComponent(Entity e)
		{
			return GetPool<T>().Get(e);
		}
		
		template<typename T>
		void RemoveComponent(Entity e)
		{
			GetPool<T>().Destroy(e);
		}

		template<typename... Components>
		View<Components...> View()
		{
			return Umi::View<Components...>(*this);
		}

		Registry() = default;

		Registry(const Registry&) = delete;
		Registry& operator=(const Registry&) = delete;

		Registry(Registry&&) = default;
		Registry& operator=(Registry&&) = default;
	};

	template<typename... Components>
	class View
	{
	public:
		Registry& registry;
		std::vector<Entity> matchingEntities;

		View(Registry& reg) : registry(reg)
		{
			auto& smallestPool = GetSmallestPool<Components...>();

			for (Entity e : smallestPool)
			{
				if ((registry.HasComponent<Components>(e) && ...))
				{
					matchingEntities.push_back(e);
				}
			}
		}

		auto begin() { return matchingEntities.begin(); }
		auto end() { return matchingEntities.end(); }

	private:
		template<typename First, typename... Rest>
		const std::vector<Entity>& GetSmallestPool()
		{
			if constexpr (sizeof...(Rest) == 0)
			{
				return registry.GetPool<First>().GetEntities();
			}
			else
			{
				auto& firstPool = registry.GetPool<First>().GetEntities();
				auto& restPool = GetSmallestPool<Rest...>();
				return (firstPool.size() < restPool.size()) ? firstPool : restPool;
			}
		}
	};

}