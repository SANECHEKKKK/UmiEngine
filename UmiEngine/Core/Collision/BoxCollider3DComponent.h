#pragma once
#include <Math/Math.h>

namespace Umi
{
	struct BoxCollider3DComponent
	{
		bool isPhysics = true;
		bool isStatic = false;
		bool isTrigger = false;

		bool prevHit = false;
		bool currentHit = false;

		Vector3 pos;
		Vector3 size;

		BoxCollider3DComponent() : pos(0, 0, 0), size(1, 1, 1) {}
		BoxCollider3DComponent(const Vector3& pos, const Vector3& size, bool isPhysics = true, bool isTrigger = false, bool isStatic = false)
			: pos(pos), size(size), isPhysics(isPhysics), isStatic(isStatic), isTrigger(isTrigger) {}
	};
}