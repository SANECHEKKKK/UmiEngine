module Mouse;

using namespace Umi;

void Mouse::Update() noexcept
{
	leftIsPressedPrev = leftIsPressed;
	rightIsPressedPrev = rightIsPressed;
}


//std::pair<int, int> Mouse::GetPos() const noexcept
//{
//	return { x,y };
//}
//
//int Mouse::GetPosX() const noexcept
//{
//	return x;
//}
//
//int Mouse::GetPosY() const noexcept
//{
//	return y;
//}

bool Mouse::LeftIsPressed() noexcept
{
	return leftIsPressed;
}

bool Mouse::RightIsPressed() noexcept
{
	return rightIsPressed;
}

bool Mouse::LeftIsTriggered() noexcept
{
	if (leftIsPressed && !leftIsPressedPrev)
	{
		return true;
	}
	return false;
}

bool Mouse::RightIsTriggered() noexcept
{
	if (rightIsPressed && !rightIsPressedPrev)
	{
		return true;
	}
	return false;
}

void Mouse::OnMouseMove(int newx, int newy) noexcept
{
	x = newx;
	y = newy;
}

void Mouse::OnLeftPressed(int x, int y) noexcept
{
	leftIsPressed = true;
}

void Mouse::OnLeftReleased(int x, int y) noexcept
{
	leftIsPressed = false;
}

void Mouse::OnRightPressed(int x, int y) noexcept
{
	rightIsPressed = true;
}

void Mouse::OnRightReleased(int x, int y) noexcept
{
	rightIsPressed = false;
}

//void Mouse::OnWheelUp(int x, int y) noexcept
//{
//	buffer.push(Mouse::Event(Mouse::Event::Type::WheelUp, *this));
//	TrimBuffer();
//}
//
//void Mouse::OnWheelDown(int x, int y) noexcept
//{
//	buffer.push(Mouse::Event(Mouse::Event::Type::WheelDown, *this));
//	TrimBuffer();
//}

