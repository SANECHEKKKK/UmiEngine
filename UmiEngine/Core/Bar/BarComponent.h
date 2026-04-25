#pragma once

namespace Umi
{
	struct BarComponent
	{
		int minValue = 0;
		int maxValue = 100;
		int currentValue = 0;
		int stepSize = 1;
	};
}