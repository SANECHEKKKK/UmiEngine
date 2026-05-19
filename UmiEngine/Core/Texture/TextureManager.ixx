module;
#include <string_view>
export module TextureManager;

import Texture;

namespace Umi
{
	struct TextureData
	{

	};

	export class TextureManager
	{
	private:


	public:
			TextureID LoadTexture(std::string_view filePath);
			void UnloadTexture(TextureID id);

			TextureManager();
	};
	
}