#pragma once

namespace Umi
{
	class Registry;

	class BarSystem
	{
	private:
		Registry& registry;

	public:
		void Update();

		BarSystem(Registry& registry);
	};
}