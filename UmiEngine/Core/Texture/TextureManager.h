#pragma once
#include <string>
#include <array>
#include <vector>

class ScratchImage;
class TexMetadata;
struct ID3D11ShaderResourceView;
struct ID3D11Device;
struct ID3D11DeviceContext;

namespace Umi
{
	class TextureManager
	{
	private:
		struct TextureData
		{
			std::wstring filename;
			ID3D11ShaderResourceView* shader_resource_view = nullptr;
			bool loaded = false;
			TextureData() = default;
			~TextureData();
		};

		static constexpr int TEXTURE_MAX = 20;
		std::array<TextureData, TEXTURE_MAX> g_Textures;
		int g_TextureCount = 0;

		ID3D11Device* g_Device = nullptr;
		ID3D11DeviceContext* g_DeviceContext = nullptr;

	public:
		void Init(ID3D11Device* _g_Device, ID3D11DeviceContext* _g_DeviceContext);
		int LoadTexure(const std::wstring& filepath);
		ID3D11ShaderResourceView* Get(int id) const noexcept;
		void Apply(int id);

		void LoadMain();

		ID3D11Device* GetDevice() const noexcept { return g_Device; }
		ID3D11DeviceContext* GetDeviceContext() const noexcept { return g_DeviceContext; }

		TextureManager() = default;
		TextureManager(const TextureManager&) = delete;
		TextureManager& operator=(const TextureManager&) = delete;
		TextureManager(TextureManager&&) = delete;
		TextureManager& operator=(TextureManager&&) = delete;

		~TextureManager();
	};
}