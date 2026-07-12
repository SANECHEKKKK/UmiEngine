module;
#include <EngineApi/EngineApi.h>
export module BasicScript;

import Entity;
import Registry;
import Transform;

export namespace Umi
{
	class ENGINE_API BasicScript
	{		
	protected:
		friend class ScriptManager;

		
		Registry* registry;

		void Bind(Entity e, Registry* reg)
		{
			entity = e;
			registry = reg;
		}

		template<typename T>
		T& GetComponent()
		{
			return registry->GetComponent<T>(entity);
		}
		
		template<typename T>
		T& GetComponent(BasicScript* script)
		{
			return registry->GetComponent<T>(script->entity);
		}


	public:
		//friend class Editor;

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
}