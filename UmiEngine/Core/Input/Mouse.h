#pragma once
#include <iostream>

namespace Umi
{
	class Mouse
	{
	private:
		bool leftIsPressed = false;
		bool rightIsPressed = false;
		bool leftIsPressedPrev = false;
		bool rightIsPressedPrev = false;

	public:
		int x = 0;
		int y = 0;

		void OnMouseMove(int x, int y) noexcept;
		void OnLeftPressed(int x, int y) noexcept;
		void OnLeftReleased(int x, int y) noexcept;
		void OnRightPressed(int x, int y) noexcept;
		void OnRightReleased(int x, int y) noexcept;
		//void OnWheelUp(int x, int y) noexcept;
		//void OnWheelDown(int x, int y) noexcept;

		//std::pair<int, int> GetPos() const noexcept;
		//int GetPosX() const noexcept;
		//int GetPosY() const noexcept;
		//bool IsInWindow() const noexcept;
		bool LeftIsPressed() const noexcept;
		bool RightIsPressed() const noexcept;
		bool LeftIsTriggered() noexcept;
		bool RightIsTriggered() noexcept;

		void Update() noexcept;

		Mouse() = default;
		~Mouse() = default;
	};
}