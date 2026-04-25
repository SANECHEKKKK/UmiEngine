#pragma once
#include <DirectXMath.h>
#include <Entity/Entity.h>
#include <Math/Math.h>

namespace Umi
{
	struct EngineContext;

	enum class CameraState
	{
		Idle,
		Moving,
		Zooming
	};

	enum class CamerType
	{
		Free,
		Stationary,
		Follow
	};
	
	class Camera
	{
	private:
		CameraState state = CameraState::Idle;
		CamerType type = CamerType::Free;

		Entity target;

		DirectX::XMFLOAT3 m_position;
		DirectX::XMFLOAT3 m_target;
		DirectX::XMFLOAT3 m_up;

		float m_zoom;

		DirectX::XMMATRIX m_view;
		DirectX::XMMATRIX m_projection;

		EngineContext& engineContext;

		void UpdateViewMatrix();

	public:
		DirectX::XMFLOAT3 rotation;

		void SetState(CameraState newState) { state = newState; }
		CameraState GetState() const { return state; }

		void SetType(CamerType newType) { type = newType; }
		CamerType GetType() const { return type; }

		DirectX::XMFLOAT3 GetPosition() const { return m_position; }
		void SetPosition(const DirectX::XMFLOAT3& pos);

		void SetTarget(Entity e) { target = e; }

		void SetTargetNew(const DirectX::XMFLOAT3& targetPos) { m_target = targetPos; }

		void Move(float x, float y, float speed);

		void SetZoom(float zoom);
		float GetZoom() const { return m_zoom; }

		void UpdateProjectionMatrix(float width = 0.0f, float height = 0.0f);

		DirectX::XMMATRIX GetViewMatrix() const { return m_view; }
		DirectX::XMMATRIX GetProjectionMatrix() const { return m_projection; }
		DirectX::XMMATRIX GetViewProjectionMatrix() const { return m_view * m_projection; }

		Vector2 WorldToScreen(const Vector3& worldPos) const;

		void Update();

		Camera(EngineContext& engineContext);
	};
}