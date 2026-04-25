#pragma once
#include <Entity/Entity.h>

namespace Umi
{
	struct EditorContext
	{
		Entity selectedEntity = Umi::INVALID_ENTITY;

		EditorContext() = default;
	};
}
