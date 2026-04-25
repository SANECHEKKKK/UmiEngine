#pragma once
#include <Resolution/Resolution.h>

namespace Umi
{
	struct ResolutionChange
	{
		Resolution resolution;

		ResolutionChange() = default;
		ResolutionChange(int width, int height)
			: resolution{ width, height }
		{}
	};
}