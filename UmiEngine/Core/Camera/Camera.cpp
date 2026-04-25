#include <Camera/Camera.h>
#include <Settings/Settings.h>
#include <EngineContext/EngineContext.h>

using namespace Umi;
using DirectX::XMFLOAT3;
using DirectX::XMVECTOR;
using DirectX::XM_PIDIV4;

Camera::Camera(EngineContext& engineContext) : engineContext(engineContext)
{
	m_position = XMFLOAT3(0.0f, 0.0f, -5.0f);
	m_target = XMFLOAT3(0.0f, 0.0f, 0.0f);
	m_up = XMFLOAT3(0.0f, 1.0f, 0.0f);
	UpdateViewMatrix();

	float aspectRatio = 16.0f / 9.0f;
	m_projection = DirectX::XMMatrixPerspectiveFovLH(XM_PIDIV4, aspectRatio, 0.1f, 100.0f);

}

void Camera::UpdateViewMatrix()
{
	XMVECTOR pos = XMLoadFloat3(&m_position);
	XMVECTOR target = XMLoadFloat3(&m_target);
	XMVECTOR up = XMLoadFloat3(&m_up);
	m_view = DirectX::XMMatrixLookAtLH(pos, target, up);

}

void Camera::UpdateProjectionMatrix(float width, float height)
{
	if (width == 0.0f || height == 0.0f)
	{
		width = static_cast<float>(engineContext.settings.getResolution().width);
		height = static_cast<float>(engineContext.settings.getResolution().height);
	}
	m_projection = DirectX::XMMatrixOrthographicOffCenterLH(
		-width / 2, +width / 2,
		+height / 2, -height / 2,
		0.0f, 1.0f
	);

	UpdateViewMatrix();

	float aspectRatio = width / height;
	m_projection = DirectX::XMMatrixPerspectiveFovLH(XM_PIDIV4, aspectRatio, 0.1f, 100.0f);

}

void Camera::SetPosition(const DirectX::XMFLOAT3& pos) {
	m_position = pos;
	UpdateViewMatrix();
}

void Camera::Move(float x, float y, float speed)
{
	return;
}

void Camera::SetZoom(float zoom)
{
	m_zoom = zoom > 0.1f ? zoom : 0.1f;
	UpdateViewMatrix();
}

Vector2 Camera::WorldToScreen(const Vector3& worldPos) const
{
	XMVECTOR worldPosition = DirectX::XMLoadFloat3(reinterpret_cast<const XMFLOAT3*>(&worldPos));
	XMVECTOR clipSpacePos = DirectX::XMVector3Transform(worldPosition, GetViewProjectionMatrix());
	XMVECTOR ndcPos = DirectX::XMVectorDivide(clipSpacePos, DirectX::XMVectorSplatW(clipSpacePos));
	float screenX = ((DirectX::XMVectorGetX(ndcPos) + 1.0f) / 2.0f) * engineContext.settings.getResolution().width;
	float screenY = ((1.0f - DirectX::XMVectorGetY(ndcPos)) / 2.0f) * engineContext.settings.getResolution().height;
	return Vector2(screenX, screenY);
}

void Camera::Update()
{
	switch (type)
	{
	case CamerType::Free:
		break;
	case CamerType::Stationary:
		break;
	case CamerType::Follow:
		if (target) {
			//m_position = target->getPos();
			UpdateViewMatrix();
		}
		break;
	default:
		break;
	}
}

//void Camera:: Move(float dx, float dy) {
//	m_position.x += dx;
//	m_position.y += dy;
//	UpdateViewMatrix();
//}