module;
#include <EngineApi/EngineApi.h>
export module ColliderBox;

import Math;

export namespace Umi
{
	struct ENGINE_API ColliderBox
	{
		bool isStatic{ false };

		Vector3 size{ 1.0f, 1.0f, 1.0f };
		
		Vector3 localPosition{ 0.0f, 0.0f, 0.0f };
		Vector3 localRotation{ 0.0f, 0.0f, 0.0f };
		Vector3 localScale{ 1.0f, 1.0f, 1.0f };

		Vector3 worldPosition{ 0.0f, 0.0f, 0.0f };
		Vector3 worldRotation{ 0.0f, 0.0f, 0.0f };
	};
}