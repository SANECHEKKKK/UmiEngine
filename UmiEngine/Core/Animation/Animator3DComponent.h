#pragma once
#include <string>
#include <vector>
#include <unordered_map>

#include <Animation/AnimationState.h>

#include <DirectXMath.h>

namespace Umi
{
	struct Model;
	struct Animation3D;

	struct Animator3DComponent
	{
		Model* model;

		int rootNodeIndex = 0;

		float currentTime = 0.0f;
		float nextTime = 0.0f;

		Animation3D* currentAnimation = nullptr;
		Animation3D* nextAnimation = nullptr;

		bool isBlending = false;
		float blendTime = 0.0f;
		float blendDuration = 0.3f;

		std::vector<DirectX::XMMATRIX> finalBoneMatrices;

		std::unordered_map<StringID, std::string> animNames;

		Animator3DComponent();
	};
}