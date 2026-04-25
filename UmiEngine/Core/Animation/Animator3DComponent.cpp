#include <Animation/Animator3DComponent.h>
#include <DirectXMath.h>

static constexpr int MAX_BONES = 100;

Umi::Animator3DComponent::Animator3DComponent()
{
	finalBoneMatrices.resize(MAX_BONES);
	for (int i = 0; i < MAX_BONES; i++)
	{
		finalBoneMatrices[i] = DirectX::XMMatrixIdentity();
	}
}
