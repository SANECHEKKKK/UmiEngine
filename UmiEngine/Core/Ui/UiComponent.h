#pragma once

namespace Umi
{
	enum class Anchor
	{
		TopLeft,		TopCenter,		TopRight,
		MiddleLeft,		MiddleCenter,	MiddleRight,
		BottomLeft,		BottomCenter,	BottomRight
	};

	struct UiComponent
	{
		Anchor anchor = Anchor::MiddleCenter;

		float width = 1.0f;
		float height = 1.0f;

		float offsetX = 0.0f;
		float offsetY = 0.0f;
	};
}