module;
#include <DirectXMath.h>
export module Camera;

export namespace Umi
{
	struct Camera
	{
		float fov = 90.0f;
		float nearClip = 0.1f;
		float farClip = 1000.0f;

		DirectX::XMMATRIX viewMatrix2D;
		DirectX::XMMATRIX projectionMatrix2D;

		DirectX::XMMATRIX viewMatrix3D;
		DirectX::XMMATRIX projectionMatrix3D;
	};
}