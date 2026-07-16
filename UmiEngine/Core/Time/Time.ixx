module;
#include <EngineApi/EngineApi.h>
export module Time;

export namespace Umi
{
	struct ENGINE_API Time
	{
		static float deltaTime;
		static float totalTime;
	};
}

float Umi::Time::deltaTime = 0.0f;