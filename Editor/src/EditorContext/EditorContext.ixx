export module EditorContext;

import Entity;

export namespace Umi
{
	struct EditorContext
	{
		Entity selectedEntity { INVALID_ENTITY };
	};
}