#pragma once
#include <Texture/Rect.h>

namespace Umi
{
	using TextureID = int;

	struct TextureComponent
	{
		TextureID id = -1;
		Rect rect;
		float alpha = 1.0f;

		TextureComponent() = default;
		TextureComponent(TextureID textureID) : id(textureID) {}
		~TextureComponent() = default;
	};
}