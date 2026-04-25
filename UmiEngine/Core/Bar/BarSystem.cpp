#include <Bar/BarSystem.h>
#include <Bar/BarComponent.h>
#include <Registry/Registry.h>

using namespace Umi;

void BarSystem::Update()
{
	for (auto e : registry.View<BarComponent>())
	{
		auto& bar = registry.GetComponent<BarComponent>(e);
		if (bar.currentValue < bar.minValue)
		{
			bar.currentValue = bar.minValue;
		}
		else if (bar.currentValue > bar.maxValue)
		{
			bar.currentValue = bar.maxValue;
		}
	}
}


BarSystem::BarSystem(Registry& registry) : registry(registry) {}
