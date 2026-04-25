#pragma once
#include <Texture/TextureComponent.h>

namespace Umi
{
	enum ButtonState
	{
		Normal,
		Hovered,
		Pressed,
		Disabled,

		Count
	};

	struct ButtonComponent
	{
		ButtonState state = ButtonState::Normal;

		//Textures for different button states
		TextureID textures[ButtonState::Count] = { -1, -1, -1, -1 };
	};
}