module;
#include <DirectXMath.h>
module CameraManager;

//import Registry;
import Camera;
import Transform;
import Keyboard;

using namespace Umi;

void CameraManager::Update()
{
	for (auto e : registry.View<Camera, Transform>())
	{

		auto& camera = registry.GetComponent<Camera>(e);
		auto& transform = registry.GetComponent<Transform>(e);

		//----------------------------2D-----------------------------
		{
			camera.projectionMatrix2D = DirectX::XMMatrixOrthographicLH(
				1200.0f,
				720.0f,
				camera.nearClip,
				camera.farClip
			);

			DirectX::XMFLOAT3 eye(transform.pos.x, transform.pos.y, -1);
			DirectX::XMFLOAT3 target(transform.pos.x, transform.pos.y, 0.0f);
			DirectX::XMFLOAT3 up(0, 1, 0);
			camera.viewMatrix2D = DirectX::XMMatrixLookAtLH(
				DirectX::XMLoadFloat3(&eye),
				DirectX::XMLoadFloat3(&target),
				DirectX::XMLoadFloat3(&up)
			);
		}
		//-----------------------------------------------------------

	//----------------------------3D-----------------------------
		{
			camera.projectionMatrix3D = DirectX::XMMatrixPerspectiveFovLH(
				DirectX::XMConvertToRadians(camera.fov),
				1200.0f / 720.0f,
				camera.nearClip,
				camera.farClip
			);

			DirectX::XMMATRIX rotMatrix = DirectX::XMMatrixRotationRollPitchYaw(
				transform.rot.x,  // pitch
				transform.rot.y,  // yaw
				transform.rot.z   // roll
			);

			// Extract axes from the rotation matrix
			DirectX::XMVECTOR forward = DirectX::XMVector3TransformNormal(
				DirectX::XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f), rotMatrix);
			DirectX::XMVECTOR up = DirectX::XMVector3TransformNormal(
				DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f), rotMatrix);

			DirectX::XMVECTOR eyePos = DirectX::XMVectorSet(
				transform.pos.x, transform.pos.y, transform.pos.z, 0.0f);

			camera.viewMatrix3D = DirectX::XMMatrixLookToLH(eyePos, forward, up);
		}
		//-----------------------------------------------------------

		if (Keyboard::IsKeyDown(KK_W))
		{
			transform.pos.z += 0.1f;
		}
		if (Keyboard::IsKeyDown(KK_S))
		{
			transform.pos.z -= 0.1f;
		}
		if (Keyboard::IsKeyDown(KK_A))
		{
			transform.pos.x -= 0.1f;
		}
		if (Keyboard::IsKeyDown(KK_D))
		{
			transform.pos.x += 0.1f;
		}
	}
}

Camera* CameraManager::GetMainCamera()
{
	for (auto e : registry.View<Camera>())
	{
		auto& camera = registry.GetComponent<Camera>(e);
		return &camera;
	}
	return nullptr;
}