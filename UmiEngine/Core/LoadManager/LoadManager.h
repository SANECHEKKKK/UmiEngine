#pragma once
#include <thread>
#include <Signal/Signal.h>

namespace Umi
{
	struct EngineContext;

	class LoadManager
	{
	private:
		std::thread worker;
		std::atomic<bool> finished{ false };
		std::atomic<uint32_t> completedSteps{ 0 };
		uint32_t totalSteps = 0;
		
		EngineContext& engineContext;

	public:
		void LoadThread(struct PrefabRequest request);
		Umi::Signal Update();
		bool IsFinished() const noexcept { return finished; };
		int GetTotalSteps() const noexcept { return totalSteps; };
		int GetCompletedSteps() const noexcept { return completedSteps; };
		float GetProgress() const;

		LoadManager(EngineContext& engineContext);
		~LoadManager();
	};
}