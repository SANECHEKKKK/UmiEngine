#pragma once
#include <array>

namespace Umi
{
	struct Resolution
	{
		int width;
		int height;
	};

	const std::array<Resolution, 7> resolutions = {
			Resolution(1280, 720),
			Resolution(1366, 768),
			Resolution(1440, 900),
			Resolution(1536, 864),
			Resolution(1600, 900),
			Resolution(1920, 1080),
			Resolution(2560, 1440)
	};

}