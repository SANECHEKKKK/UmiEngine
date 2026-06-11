module;
#include <EngineApi/EngineApi.h>
export module BasicScript;

import Entity;
import EngineContext;
import Transform;

export namespace Umi
{
	class ENGINE_API BasicScript
	{
	protected:
		EngineContext* engineContext;

		void Bind(Entity e, EngineContext* ctx)
		{
			entity = e;
			engineContext = ctx;
		}

		template<typename T>
		T& GetComponent()
		{
			return engineContext->registry.GetComponent<T>(entity);
		}
		
		template<typename T>
		T& GetComponent(BasicScript* script)
		{
			return engineContext->registry.GetComponent<T>(script->entity);
		}


	public:
		friend class Editor;

		Entity entity;

		Transform& transform() { return engineContext->registry.GetComponent<Transform>(entity); };	

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