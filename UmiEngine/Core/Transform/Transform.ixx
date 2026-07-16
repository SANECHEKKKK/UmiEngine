module;
#include <EngineApi/EngineApi.h>
#include <cmath>
#include <DirectXMath.h>
export module Transform;

export import Math;

export namespace Umi
{
	struct ENGINE_API Transform
	{
		Vector3 pos;
		Vector3 rot;	// in degrees
		Vector3 scale;

		Vector3 Forward() const
		{
			float yaw = DirectX::XMConvertToRadians(rot.y);
			return { std::sin(yaw), 0.0f, std::cos(yaw) };
		}

		Transform() : pos(0.0f, 0.0f, 0.0f), scale(1.0f, 1.0f, 1.0f), rot(0.0f, 0.0f, 0.0f) {}
		Transform(float x, float y, float z, float scaleX = 1.0f, float scaleY = 1.0f, float scaleZ = 1.0f, float rotX = 0.0f, float rotY = 0.0f, float rotZ = 0.0f)
			: pos(x, y, z), scale(scaleX, scaleY, scaleZ), rot(rotX, rotY, rotZ) {
		}
		Transform(Vector3 _position, Vector3 _scale = Vector3(1.0f, 1.0f, 1.0f), Vector3 _rotation = Vector3(0.0f, 0.0f, 0.0f))
			: pos(_position), scale(_scale), rot(_rotation) {
		}
	};
}