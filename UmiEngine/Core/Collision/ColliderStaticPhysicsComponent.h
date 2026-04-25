#pragma once
#include <Math/Math.h>

namespace Umi
{
	struct BoxColliderStatic3DComponent
	{
		bool prevHit = false;
		bool currentHit = false;

		Vector3 pos;
		Vector3 size;
	};
}