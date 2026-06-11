module;
#include <string>
#include <string_view>
#include <vector>
#include <wrl/client.h>

#include <Graphics/d3dx12.h>

#include <EngineApi/EngineApi.h>
export module TextureManager;

import Texture;
import GraphicsContext;
import DescriptorHeap;

using Microsoft::WRL::ComPtr;

export namespace Umi
{
	struct ENGINE_API TextureData
	{
		ComPtr<ID3D12Resource> texBuff = nullptr;
		std::string filePath;
		UINT descriptorHeapIndex = 0;
		float width, height;
	};

	class ENGINE_API TextureManager
	{
	private:
		GraphicsContext& graphicsContext;

		std::vector<TextureData> textureList;

	public:
		[[nodiscard]] TextureID LoadTexture(std::string_view filePath, DescriptorHeap& descriptorHeap);

		//-----------------------TEMPROARY-----------------------
		[[nodiscard]] TextureID LoadTexture2D(std::string_view filePath);
		[[nodiscard]] TextureID LoadTexture3D(std::string_view filePath);
		[[nodiscard]] TextureID LoadTexture3DRawData(std::vector<uint8_t> data, int width, int height);
		[[nodiscard]] TextureID LoadTexture3DRawData(uint8_t* data, int width, int height);
		//-----------------------TEMPROARY-----------------------

		void UnloadTexture(TextureID id);
		TextureData& GetTextureData(TextureID id);

		TextureManager(GraphicsContext& graphicsContext) : graphicsContext(graphicsContext) 
		{
			textureList.reserve(64);
		};
	};

}