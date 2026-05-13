export module EditorContext;

import Entity;

namespace Umi
{
	struct EditorContext
	{
		Entity selectedEntity = Umi::INVALID_ENTITY;

		EditorContext() = default;
	};
}