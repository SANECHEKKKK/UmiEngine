module;
#include <string>
#include <string_view>
#include <vector>
#include <wrl/client.h>

#include <Graphics/d3dx12.h>
export module TextureManager;

import Texture;
import GraphicsContext;

using Microsoft::WRL::ComPtr;

export namespace Umi
{
	struct TextureData
	{
		ComPtr<ID3D12Resource> texBuff = nullptr;
		std::string filePath;
		UINT descriptorHeapIndex = 0;
	};

	class TextureManager
	{
	private:
		GraphicsContext& graphicsContext;

		std::vector<TextureData> textureList;

	public:
		TextureID LoadTexture(std::string_view filePath);
		void UnloadTexture(TextureID id);
		TextureData& GetTextureData(TextureID id);

		TextureManager(GraphicsContext& graphicsContext) : graphicsContext(graphicsContext) 
		{
			textureList.reserve(64);
		};
	};

}