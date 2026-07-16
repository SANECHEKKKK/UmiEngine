module;
#include <DirectXMath.h>
#include <numbers>
#include <cmath>

#include <EngineApi/EngineApi.h>
export module Math;
//export module Vector;

export namespace Umi
{
	constexpr double PI = std::numbers::pi;
	constexpr double TWO_PI = 2 * std::numbers::pi;

	struct ENGINE_API float3
	{
		float x, y, z;
		float3() : x(0), y(0), z(0) {}
		float3(float x, float y, float z) : x(x), y(y), z(z) {}
		//Struct operators//
		operator struct DirectX::XMFLOAT3() const
		{
			return DirectX::XMFLOAT3(x, y, z);
		}
	};


	struct ENGINE_API float4
	{
		float x, y, z, w;
		float4() : x(0), y(0), z(0), w(0) {}
		float4(float x, float y, float z, float w) : x(x), y(y), z(z), w(w) {}
		//Struct operators//
		operator struct DirectX::XMFLOAT4() const
		{
			return DirectX::XMFLOAT4(x, y, z, w);
		}
	};


	struct ENGINE_API Vector2
	{
		float x, y;

		Vector2() : x(0), y(0) {}

		Vector2(float x, float y) : x(x), y(y) {}

		//------------------OPERATORS------------------//
		Vector2 operator+(const Vector2& other) const {
			return Vector2(x + other.x, y + other.y);
		}

		Vector2 operator-(const Vector2& other) const {
			return Vector2(x - other.x, y - other.y);
		}

		Vector2 operator*(float scalar) const {
			return Vector2(x * scalar, y * scalar);
		}

		Vector2 operator/(float scalar) const {
			if (scalar != 0) {
				return Vector2(x / scalar, y / scalar);
			}
			return Vector2(0, 0);
		}

		Vector2 operator=(const Vector2& other) {
			x = other.x;
			y = other.y;
			return *this;
		}

		Vector2 operator=(const float& other)
		{
			x = other;
			y = other;
			return *this;
		}

		Vector2 operator-() const {
			return Vector2(-x, -y);
		}

		Vector2 operator+=(const Vector2& other) {
			x += other.x;
			y += other.y;
			return *this;
		}

		Vector2 operator+=(const float other) {
			x += other;
			y += other;
			return *this;
		}

		Vector2 operator-=(const Vector2& other) {
			x -= other.x;
			y -= other.y;
			return *this;
		}

		Vector2 operator-=(const float other) {
			x -= other;
			y -= other;
			return *this;
		}

		Vector2 operator*=(float scalar) {
			x *= scalar;
			y *= scalar;
			return *this;
		}

		Vector2 operator*=(const Vector2& other) {
			x *= other.x;
			y *= other.y;
			return *this;
		}

		Vector2 operator/=(float scalar) {
			if (scalar != 0) {
				x /= scalar;
				y /= scalar;
			}
			return *this;
		}

		Vector2 operator/=(const Vector2& other) {
			if (other.x != 0 && other.y != 0) {
				x /= other.x;
				y /= other.y;
			}
			return *this;
		}

		bool operator==(const Vector2& other) const {
			return (x == other.x && y == other.y);
		}

		bool operator!=(const Vector2& other) const {
			return !(*this == other);
		}

		//Struct operators//
		operator struct DirectX::XMFLOAT2() const
		{
			return DirectX::XMFLOAT2(x, y);
		}
		//---------------------------------------------//




		//------------------FUNCTIONS------------------//
		void normalize() noexcept
		{
			float length = std::sqrt(x * x + y * y);
			if (length > 0.0f) {
				x /= length;
				y /= length;
			}
		}

		Vector2 normalized() const noexcept
		{
			float length = std::sqrt(x * x + y * y);
			if (length > 0.0f) {
				return Vector2(x / length, y / length);
			}
			return Vector2(0.0f, 0.0f);
		}

		float length() const noexcept
		{
			return std::sqrt(x * x + y * y);
		}
		//------------------FUNCTIONS---------------------------//
	};


	struct ENGINE_API Vector3
	{
		float x, y, z;

		Vector3() : x(0), y(0), z(0) {}

		Vector3(float x, float y, float z) : x(x), y(y), z(z) {}

		//------------------OPERATORS------------------//
		Vector3 operator+(const Vector3& other) const {
			return Vector3(x + other.x, y + other.y, z + other.z);
		}

		Vector3 operator-(const Vector3& other) const {
			return Vector3(x - other.x, y - other.y, z - other.z);
		}

		Vector3 operator*(float scalar) const {
			return Vector3(x * scalar, y * scalar, z * scalar);
		}

		Vector3 operator/(float scalar) const {
			if (scalar != 0) {
				return Vector3(x / scalar, y / scalar, z / scalar);
			}
			return Vector3(0, 0, 0);
		}

		Vector3 operator=(const Vector3& other) {
			x = other.x;
			y = other.y;
			z = other.z;
			return *this;
		}

		Vector3 operator=(const float& other)
		{
			x = other;
			y = other;
			z = other;
			return *this;
		}

		Vector3 operator-() const {
			return Vector3(-x, -y, -z);
		}

		Vector3 operator+=(const Vector3& other) {
			x += other.x;
			y += other.y;
			z += other.z;
			return *this;
		}

		Vector3 operator+=(const float other) {
			x += other;
			y += other;
			z += other;
			return *this;
		}

		Vector3 operator-=(const Vector3& other) {
			x -= other.x;
			y -= other.y;
			z -= other.z;
			return *this;
		}

		Vector3 operator-=(const float other) {
			x -= other;
			y -= other;
			z -= other;
			return *this;
		}

		Vector3 operator*=(float scalar) {
			x *= scalar;
			y *= scalar;
			z *= scalar;
			return *this;
		}

		Vector3 operator*=(const Vector3& other) {
			x *= other.x;
			y *= other.y;
			z *= other.z;
			return *this;
		}

		Vector3 operator/=(float scalar) {
			if (scalar != 0) {
				x /= scalar;
				y /= scalar;
				z /= scalar;
			}
			return *this;
		}

		Vector3 operator/=(const Vector3& other) {
			if (other.x != 0 && other.y != 0 && other.z != 0) {
				x /= other.x;
				y /= other.y;
				z /= other.z;
			}
			return *this;
		}

		operator Vector2() const {
			return Vector2(x, y);
		}

		bool operator==(const Vector3& other) const {
			return (x == other.x && y == other.y && z == other.z);
		}

		bool operator!=(const Vector3& other) const {
			return !(*this == other);
		}

		operator struct DirectX::XMFLOAT3() const
		{
			return DirectX::XMFLOAT3(x, y, z);
		}
		//---------------------------------------------//




		//------------------FUNCTIONS------------------//
		void normalize() noexcept
		{
			float length = std::sqrt(x * x + y * y + z * z);
			if (length > 0.0f) {
				x /= length;
				y /= length;
				z /= length;
			}
		}
		
		Vector3 normalized() const noexcept
		{
			float length = std::sqrt(x * x + y * y + z * z);
			if (length > 0.0f) {
				return Vector3(x / length, y / length, z / length);
			}
			return Vector3(0.0f, 0.0f, 0.0f);
		}
		
		float length() const noexcept
		{
			return std::sqrt(x * x + y * y + z * z);
		}
		//---------------------------------------------//
	};
}