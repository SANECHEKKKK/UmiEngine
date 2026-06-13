module;
#include <DirectXMath.h>
module CameraManager;

//import Registry;
import Camera;
import Transform;
import Keyboard;

using namespace Umi;
import Settings;

void CameraManager::Update()
{
	const float w = static_cast<float>(viewportWidth);
	const float h = static_cast<float>(viewportHeight);
	const float aspect = (h > 0.0f) ? (w / h) : 1.0f;

	for (auto e : registry.View<Camera, Transform>())
	{
		auto& camera = registry.GetComponent<Camera>(e);
		auto& transform = registry.GetComponent<Transform>(e);

		//----------------------------2D-----------------------------
		{
			camera.projectionMatrix2D = DirectX::XMMatrixOrthographicLH(
				w, h, camera.nearClip, camera.farClip);

			DirectX::XMFLOAT3 eye(transform.pos.x, transform.pos.y, -1);
			DirectX::XMFLOAT3 target(transform.pos.x, transform.pos.y, 0.0f);
			DirectX::XMFLOAT3 up(0, 1, 0);
			camera.viewMatrix2D = DirectX::XMMatrixLookAtLH(
				DirectX::XMLoadFloat3(&eye),
				DirectX::XMLoadFloat3(&target),
				DirectX::XMLoadFloat3(&up));
		}
		//-----------------------------------------------------------

		//----------------------------3D-----------------------------
		{
			camera.projectionMatrix3D = DirectX::XMMatrixPerspectiveFovLH(
				DirectX::XMConvertToRadians(camera.fov),
				aspect,
				camera.nearClip,
				camera.farClip);

			DirectX::XMMATRIX rotMatrix = DirectX::XMMatrixRotationRollPitchYaw(
				transform.rot.x, transform.rot.y, transform.rot.z);

			DirectX::XMVECTOR forward = DirectX::XMVector3TransformNormal(
				DirectX::XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f), rotMatrix);
			DirectX::XMVECTOR up = DirectX::XMVector3TransformNormal(
				DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f), rotMatrix);

			DirectX::XMVECTOR eyePos = DirectX::XMVectorSet(
				transform.pos.x, transform.pos.y, transform.pos.z, 0.0f);

			camera.viewMatrix3D = DirectX::XMMatrixLookToLH(eyePos, forward, up);
		}
		//-----------------------------------------------------------
	}
}

void CameraManager::SetViewportSize(uint32_t width, uint32_t height)
{
	if (width == 0 || height == 0) return;
	viewportWidth = width;
	viewportHeight = height;
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