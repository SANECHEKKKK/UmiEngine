#pragma once
#include <Texture/TextureComponent.h>

namespace Umi
{
	namespace CheckBoxState
	{
		enum States
		{
			Unchecked,
			Checked,

			Count
		};
	}
	struct CheckBoxComponent
	{
		CheckBoxState::States state = CheckBoxState::Unchecked;

		bool isChecked = false;

		//Textures for different checkbox states
		TextureID textures[CheckBoxState::Count] = { -1, -1 };
	};
}