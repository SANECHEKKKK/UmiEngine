#pragma once
#include <string>
#include <DirectXMath.h>

namespace Umi
{
	enum class Alignment
	{
		Left,   // Left
		Right,  // Right
		Center
	};

	enum class FontWeight
	{
		Normal,
		Bold,
		Light
	};

	enum class FontStyle
	{
		Normal,
		Italic,
		Oblique
	};

	enum class FontStretch
	{
		Normal,
		Condensed,
		Expanded
	};

	struct TextComponent
	{
		std::wstring text;

		std::wstring fontName = L"Arial";
		float fontSize = 24.0f;
		DirectX::XMFLOAT4 color = { 1.0f, 1.0f, 1.0f, 1.0f };

		float offsetX = 0.0f;
		float offsetY = 0.0f;

		Alignment horizontalAlignment = Alignment::Center;
		Alignment verticalAlignment = Alignment::Center;

		FontWeight fontWeight = FontWeight::Normal;
		FontStyle fontStyle = FontStyle::Normal;
		FontStretch fontStretch = FontStretch::Normal;
	};
}