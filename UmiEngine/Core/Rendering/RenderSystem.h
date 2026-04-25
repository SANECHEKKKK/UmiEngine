#pragma once
#include <memory>
#include <vector>
#include <Windows.h>
#include <StateRegistry/StateID.h>
#include <Entity/Entity.h>

namespace Umi
{
	class Registry;
	class Renderer;

	class RenderSystem
	{
	private:
		Registry& registry;
		Renderer& renderer;

		struct RenderItem
		{
			Umi::Entity entity;
			int layer;

			RenderItem(Umi::Entity e, int l) : entity(e), layer(l) {}
		};

		std::vector<RenderItem> renderlist;

		static inline size_t renderListReserveSize = 50;

	public:
		void Render(StringID stateID);

		RenderSystem(Registry& registry, Renderer& renderer);
		~RenderSystem() = default;
	};
}