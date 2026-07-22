module;
#include <vector>
#include <memory>
#include <string>
#include <EngineApi/EngineApi.h>
export module Script;

import Entity;
import Registry;
import Transform;
import CollisionManager;
import ID;
import EventManager;

export namespace Umi
{
	class ENGINE_API BasicScript
	{
		//-----------------BASE-----------------
	private:
		CollisionManager* collisionManager;
		EventManager* eventManager;

	protected:
		Registry* registry;

		friend class ScriptManager;

		void Bind(Entity e, Registry* reg, CollisionManager* colManager, EventManager* evManager)
		{
			entity = e;
			registry = reg;
			collisionManager = colManager;
			eventManager = evManager;
		}
		//--------------------------------------

		//------------COMPONENT FUNC------------
		template<typename T>
		bool HasComponent(Entity e)
		{
			return registry->HasComponent<T>(e);
		}

		template<typename T>
		T& GetComponent()
		{
			return registry->GetComponent<T>(entity);
		}

		template<typename T>
		T& GetComponent(Entity e)
		{
			return registry->GetComponent<T>(e);
		}

		template<typename T>
		T& GetComponent(BasicScript* script)
		{
			return registry->GetComponent<T>(script->entity);
		}
		//--------------------------------------

		//-------------DESTROY FUNC-------------
		void DestroyEntity(Entity e)
		{
			registry->DestroyEntity(e);
		}

		void DestroyEntity()
		{
			registry->DestroyEntity(entity);
		}
		//--------------------------------------

		//---------------TAG FUNC---------------
		bool HasTag(Entity e, const std::string_view& tag)
		{
			if (registry->HasComponent<ID>(e))
			{
				auto& id = registry->GetComponent<ID>(e);
				if (id.tag == tag)
				{
					return true;
				}
			}

			return false;
		}

		std::vector<Entity> GetEntitiesWithTag(std::string_view tag)
		{
			std::vector<Entity> entitesList;
			for (auto e : registry->View<ID>())
			{
				if (registry->GetComponent<ID>(e).tag == tag)
					entitesList.push_back(e);
			}
			return entitesList;
		}

		Entity FindEntityWithTag(std::string_view tag)
		{
			for (auto e : registry->View<ID>())
			{
				if (registry->GetComponent<ID>(e).tag == tag)
					return e;
			}
			return INVALID_ENTITY;
		}
		//--------------------------------------

		//------------COLLISION FUNC------------
		std::vector<Entity> CheckOverlap(const Vector3& center, const Vector3& size)
		{
			return collisionManager->OverlapBox(center, size);
		}
		//--------------------------------------

		//-------------SCRIPTS FUNC-------------
		template<typename T>
		T* GetScript(Entity e);
		//--------------------------------------

		//--------------LEVEL FUNC--------------
		void OpenLevel(std::string LevelName)
		{
			eventManager->PushOpenLevelEvent(LevelName);
		}
		//--------------------------------------

		//---------------GAME FUNC--------------
		void CloseGame()
		{
			eventManager->PushCloseGameEvent();
		}
		//--------------------------------------

	public:
		Entity entity;

		Transform& transform() { return registry->GetComponent<Transform>(entity); };

		virtual void Start() {};
		virtual void Update() {};

		BasicScript() = default;
		virtual ~BasicScript() = default;

		BasicScript(const BasicScript&) = delete;
		BasicScript& operator=(const BasicScript&) = delete;

		BasicScript(BasicScript&&) = default;
		BasicScript& operator=(BasicScript&&) = default;
	};

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

	template<typename T>
	T* BasicScript::GetScript(Entity e)
	{
		static_assert(std::derived_from<T, BasicScript>, "T must derive from BasicScript");
		auto& scripts = registry->GetComponent<Scripts>(e);
		for (auto& s : scripts.scripts)
			if (T* casted = dynamic_cast<T*>(s.instance.get()))
				return casted;
		return nullptr;
	}
}