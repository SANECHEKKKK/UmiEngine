#pragma once
#include <Math/Math.h>


namespace Umi
{
	struct Transform2DComponent
	{
		Vector2 pos;
		Vector2 size;	// Size in 2D space
		Vector2 scale;
		float rot;		// in radians

		Transform2DComponent() : pos(0.0f, 0.0f), size(1.0f, 1.0f), scale(1.0f, 1.0f), rot(0.0f) {}
		Transform2DComponent(float x, float y, float width = 1.0f, float height = 1.0f, float scaleX = 1.0f, float scaleY = 1.0f, float rotation = 0.0f)
			: pos(x, y), size(width, height), scale(scaleX, scaleY), rot(rotation) {
		}
		Transform2DComponent(Vector2 _position, Vector2 _size = Vector2(1.0f, 1.0f), Vector2 _scale = Vector2(1.0f, 1.0f), float _rotation = 0.0f)
			: pos(_position), size(_size), scale(_scale), rot(_rotation) {
		}

		float getLeft() const noexcept { return pos.x - (size.x * scale.x) / 2.0f; }
		float getRight() const noexcept { return pos.x + (size.x * scale.x) / 2.0f; }
		float getTop() const noexcept { return pos.y - (size.y * scale.y) / 2.0f; }
		float getBottom() const noexcept { return pos.y + (size.y * scale.y) / 2.0f; }
	};

	struct Transform3DComponent
	{
		Vector3 pos;
		Vector3 rot;	// in radians
		Vector3 scale;

		Transform3DComponent() : pos(0.0f, 0.0f, 0.0f), scale(1.0f, 1.0f, 1.0f), rot(0.0f, 0.0f, 0.0f) {}
		Transform3DComponent(float x, float y, float z, float scaleX = 1.0f, float scaleY = 1.0f, float scaleZ = 1.0f, float rotX = 0.0f, float rotY = 0.0f, float rotZ = 0.0f)
			: pos(x, y, z), scale(scaleX, scaleY, scaleZ), rot(rotX, rotY, rotZ) {
		}
		Transform3DComponent(Vector3 _position, Vector3 _scale = Vector3(1.0f, 1.0f, 1.0f), Vector3 _rotation = Vector3(0.0f, 0.0f, 0.0f))
			: pos(_position), scale(_scale), rot(_rotation) {
		}
	};

	struct TransformUiComponent
	{
		Vector2 pos;
		Vector2 size;	// Size in 2D space
		Vector2 scale;
		float rot;		// in radians

		TransformUiComponent() : pos(0.0f, 0.0f), size(1.0f, 1.0f), scale(1.0f, 1.0f), rot(0.0f) {}
		TransformUiComponent(float x, float y, float width = 1.0f, float height = 1.0f, float scaleX = 1.0f, float scaleY = 1.0f, float rotation = 0.0f)
			: pos(x, y), size(width, height), scale(scaleX, scaleY), rot(rotation) {
		}
		TransformUiComponent(Vector2 _position, Vector2 _size = Vector2(1.0f, 1.0f), Vector2 _scale = Vector2(1.0f, 1.0f), float _rotation = 0.0f)
			: pos(_position), size(_size), scale(_scale), rot(_rotation) {
		}

		float getLeft() const noexcept;
		float getRight() const noexcept;
		float getTop() const noexcept;
		float getBottom() const noexcept;
	};
}