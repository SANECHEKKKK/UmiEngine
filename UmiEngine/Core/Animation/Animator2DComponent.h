#pragma once
#include <vector>
#include <Texture/Rect.h>

namespace Umi
{
	struct Animation2DFrame
	{
		Rect rect;
		float duration = 1.0f;
	};

	struct Animation2D
	{
		std::vector<Animation2DFrame> frames; //Frames of the animation

		Animation2D() = default;
		Animation2D(const std::vector< Animation2DFrame>& frames) : frames(frames) {};
		//Animation2D(const int rowNuber, const int columnNumber, const int startFrame, const int endFrame, const float frameTime = 100.0f);
		~Animation2D() = default;
	};

	struct Animator2D
	{
		std::vector<Animation2D> amimations;
		Animation2D& currentAnimation;
		int currentFrameIndex = 0;
		float elapsedTime = 0.0f;
	};
}