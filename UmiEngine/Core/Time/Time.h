#pragma once

namespace Umi
{
	struct Time
	{
		static float deltaTime;
		static float totalTime;
		static void Update()
		{
			totalTime += deltaTime;
		}
		static float GetDeltaTime()
		{
			return deltaTime;
		}
		static float GetTotalTime()
		{
			return totalTime;
		}
	};

	class Timer
	{
	private:
		float time;
		float duration;
		bool isRunning;

	public:
		void Start()
		{
			time = 0.0f;
			isRunning = true;
		}
		void Stop()
		{
			isRunning = false;
		}
		void Update()
		{
			if (isRunning)
			{
				time += Time::GetDeltaTime();
				if (time >= duration)
				{
					time = duration;
					isRunning = false;
				}
			}
		}
		inline bool IsFinished() const noexcept
		{
			return time >= duration;
		}

		Timer(float duration) : time(0.0f), duration(duration), isRunning(false) {}
	};
}