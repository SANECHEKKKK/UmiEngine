#include <Error/ErrorManager.h>
#include <Registry/Registry.h>
#include <Transform/TransformComponent.h>

using namespace Umi;

//void ErrorManager::CreateErrorEntity(const Error& error) noexcept
//{
//	Entity errorEntity = registry.CreateEntity();
//	registry.AddComponent<ErrorComponent>(errorEntity, ErrorComponent(error.message));
//	registry.AddComponent<TransformUiComponent>(errorEntity, TransformUiComponent());
//	errorEntities[errorEntity] = Timer(5.0f);
//}

void ErrorManager::Update()
{
	//for (auto& [entity, timer] : errorEntities)
	//{
	//	timer.Update();
	//	if (timer.IsFinished())
	//	{
	//		errorEntities.erase(entity);
	//	}
	//}
}

void ErrorManager::CreateError(const Error& error) noexcept
{
	//CreateErrorEntity(error);
}