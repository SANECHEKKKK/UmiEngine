#include <Math/Math.h>
#include <cmath>

using namespace Umi;

//------------------VECTOR3------------------//
void Vector3::normalize() noexcept
{
	float length = std::sqrt(x * x + y * y + z * z);
	if (length > 0.0f) {
		x /= length;
		y /= length;
		z /= length;
	}
}

Vector3 Vector3::normalized() const noexcept
{
	float length = std::sqrt(x * x + y * y + z * z);
	if (length > 0.0f) {
		return Vector3(x / length, y / length, z / length);
	}
	return Vector3(0.0f, 0.0f, 0.0f);
}

float Vector3::length() const noexcept
{
	return std::sqrt(x * x + y * y + z * z);
}
//---------------------------------------------//




//------------------VECTOR2------------------//
Vector2 Vector2::normalized() const noexcept
{
	float length = std::sqrt(x * x + y * y);
	if (length > 0.0f) {
		return Vector2(x / length, y / length);
	}
	return Vector2(0.0f, 0.0f);
}

void Vector2::normalize() noexcept
{
	float length = std::sqrt(x * x + y * y);
	if (length > 0.0f) {
		x /= length;
		y /= length;
	}
}

float Vector2::length() const noexcept
{
	return std::sqrt(x * x + y * y);
}
//---------------------------------------------//