#pragma once

namespace Umi
{
	class Registry;

	class UiSystem
	{
	private:
		Registry& registry;

	public:

		//Anchor StringToAnchor(const std::string& _anchorname);

		void RecalculateAll(const struct Resolution& resolution) noexcept;
		void CalculatePosition(struct UiComponent& uiComponent, struct TransformUiComponent& transform, const struct Resolution& resolution);

		UiSystem(Registry& registry);
		~UiSystem() = default;
	};
}