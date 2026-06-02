module;
#include <iostream>
export module Mouse;

export namespace Umi
{
	class Mouse
	{
	private:
		inline static bool leftIsPressed = false;
		inline static bool rightIsPressed = false;
		inline static bool leftIsPressedPrev = false;
		inline static bool rightIsPressedPrev = false;

	public:
		inline static int x = 0;
		inline static int y = 0;

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
		static bool LeftIsPressed() noexcept;
		static bool RightIsPressed() noexcept;
		static bool LeftIsTriggered() noexcept;
		static bool RightIsTriggered() noexcept;

		void Update() noexcept;

		Mouse() = default;
		~Mouse() = default;
	};
}