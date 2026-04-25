#include <Rendering/RenderSystem.h>

#include <Registry/Registry.h>
#include <Rendering/Renderer.h>

#include <Transform/TransformComponent.h>
#include <Texture/TextureComponent.h>
#include <Model/ModelComponent.h>
#include <Animation/Animator3DComponent.h>
#include <Tag/TagComponent.h>
#include <Bar/BarComponent.h>
#include <Layer/LayerComponent.h>
#include <Text/TextComponent.h>

#include <algorithm>

#ifdef _DEBUG
#include <Collision/BoxCollider3DComponent.h>
#endif // _DEBUG

using namespace Umi;


void RenderSystem::Render(StringID stateID)
{

	//--------------------------------------2D-----------------------------------------//
	for (auto e : registry.View<Transform2DComponent, TextureComponent, LayerComponent, StateIDComponent>())
	{
		auto& stateIDComponent = registry.GetComponent<StateIDComponent>(e);

		if (stateIDComponent.stateID != stateID)
			continue;

		renderlist.push_back({ e, registry.GetComponent<LayerComponent>(e).layer });
	}

	if (!renderlist.empty())
	{
		std::sort(renderlist.begin(), renderlist.end(), [](auto& a, auto& b) {
			return a.layer < b.layer;
			});

		for (auto& item : renderlist)
		{
			auto& transform = registry.GetComponent<Transform2DComponent>(item.entity);
			auto& textureComponent = registry.GetComponent<TextureComponent>(item.entity);

			renderer.Render2D(transform, textureComponent);
		}

		renderlist.clear();
	}

	for (auto e : registry.View<TransformUiComponent, TextureComponent, LayerComponent, StateIDComponent>())
	{
		auto& stateIDComponent = registry.GetComponent<StateIDComponent>(e);

		if (stateIDComponent.stateID != stateID)
			continue;

		renderlist.push_back({ e, registry.GetComponent<LayerComponent>(e).layer });
	}

	if (!renderlist.empty())
	{
		std::sort(renderlist.begin(), renderlist.end(), [](auto& a, auto& b) {
			return a.layer < b.layer;
			});

		for (auto& item : renderlist)
		{
			auto& transform = registry.GetComponent<TransformUiComponent>(item.entity);
			auto& textureComponent = registry.GetComponent<TextureComponent>(item.entity);

			renderer.RenderUi(transform, textureComponent);
		}

		renderlist.clear();
	}

	for (auto e : registry.View<BarComponent, TransformUiComponent, TextureComponent, LayerComponent, StateIDComponent>())
	{
		auto& stateIDComponent = registry.GetComponent<StateIDComponent>(e);
		if (stateIDComponent.stateID != stateID)
			continue;

		renderlist.push_back({ e, registry.GetComponent<LayerComponent>(e).layer });
	}

	if (!renderlist.empty())
	{
		std::sort(renderlist.begin(), renderlist.end(), [](auto& a, auto& b) {
			return a.layer < b.layer;
			});

		for (auto& item : renderlist)
		{
			auto& bar = registry.GetComponent<BarComponent>(item.entity);
			auto& transform = registry.GetComponent<TransformUiComponent>(item.entity);
			auto& texture = registry.GetComponent<TextureComponent>(item.entity);

			renderer.RenderBar(transform, texture, bar);
		}

		renderlist.clear();
	}

	for (auto e : registry.View<TransformUiComponent, TextComponent, LayerComponent, StateIDComponent>())
	{
		auto& stateIDComponent = registry.GetComponent<StateIDComponent>(e);
		if (stateIDComponent.stateID != stateID)
			continue;

		renderlist.push_back({ e, registry.GetComponent<LayerComponent>(e).layer });
	}
	
	if (!renderlist.empty())
	{
		std::sort(renderlist.begin(), renderlist.end(), [](auto& a, auto& b) {
			return a.layer < b.layer;
			});

		for (auto& item : renderlist)
		{
			TransformUiComponent transform3DToUI;
			auto& transform = registry.GetComponent<TransformUiComponent>(item.entity);
			auto& text = registry.GetComponent<TextComponent>(item.entity);

			renderer.RenderText(transform, text);
		}

		renderlist.clear();
	}
	//---------------------------------------------------------------------------------//

	//--------------------------------------3D-----------------------------------------//
	for (auto e : registry.View<Transform3DComponent, ModelComponent, StaticModelTag, StateIDComponent>())
	{
		auto& stateIDComponent = registry.GetComponent<StateIDComponent>(e);

		if (stateIDComponent.stateID != stateID)
			continue;

		auto& transform = registry.GetComponent<Transform3DComponent>(e);
		auto& modelComponent = registry.GetComponent<ModelComponent>(e);


		renderer.Render3D(transform, modelComponent);
	}

	for (auto e : registry.View<Transform3DComponent, ModelComponent, Animator3DComponent, StateIDComponent>())
	{
		auto& stateIDComponent = registry.GetComponent<StateIDComponent>(e);
		if (stateIDComponent.stateID != stateID)
			continue;

		auto& transform = registry.GetComponent<Transform3DComponent>(e);
		auto& modelComponent = registry.GetComponent<ModelComponent>(e);
		auto& animator = registry.GetComponent<Animator3DComponent>(e);

		renderer.Render3DAnimated(transform, modelComponent, animator);
	}

	for (auto e : registry.View<Transform3DComponent, TextComponent, LayerComponent, StateIDComponent>())
	{
		auto& stateIDComponent = registry.GetComponent<StateIDComponent>(e);
		if (stateIDComponent.stateID != stateID)
			continue;

		renderlist.push_back({ e, registry.GetComponent<LayerComponent>(e).layer });
	}

	if (!renderlist.empty())
	{
		std::sort(renderlist.begin(), renderlist.end(), [](auto& a, auto& b) {
			return a.layer < b.layer;
			});

		for (auto& item : renderlist)
		{
			TransformUiComponent transform3DToUI;
			auto& transform = registry.GetComponent<Transform3DComponent>(item.entity);
			auto& text = registry.GetComponent<TextComponent>(item.entity);

			renderer.RenderText(transform, text);
		}

		renderlist.clear();
	}

#ifdef _DEBUG
	for (auto e : registry.View<Transform3DComponent, BoxCollider3DComponent, StateIDComponent>())
	{
		auto& stateIDComponent = registry.GetComponent<StateIDComponent>(e);
		if (stateIDComponent.stateID != stateID)
			continue;

		auto& transform = registry.GetComponent<Transform3DComponent>(e);
		auto& collider = registry.GetComponent<BoxCollider3DComponent>(e);
		renderer.RenderBoxCollider(collider);

	}
#endif // _DEBUG
	//---------------------------------------------------------------------------------//

}

RenderSystem::RenderSystem(Registry& registry, Renderer& renderer) : registry(registry), renderer(renderer)
{
	renderlist.reserve(renderListReserveSize);
}