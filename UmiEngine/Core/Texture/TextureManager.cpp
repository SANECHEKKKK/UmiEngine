#include <Texture/TextureManager.h>
#include <d3d11.h>
#include <DirectXTex/DirectXTex.h>

using namespace DirectX;

Umi::TextureManager::TextureData::~TextureData()
{
	if (shader_resource_view)
	{
		shader_resource_view->Release();
		shader_resource_view = nullptr;
	}
}

void Umi::TextureManager::Init(ID3D11Device* _g_Device, ID3D11DeviceContext* _g_DeviceContext)
{
	g_Device = _g_Device;
	g_DeviceContext = _g_DeviceContext;
}

int Umi::TextureManager::LoadTexure(const std::wstring& filePath)
{
	// すでに同名のテクスチャが読み込まれていないか確認する
	for (int i = 0; i < g_TextureCount; i++) {
		if (g_Textures[i].filename == filePath) {
			return i; // すでに読み込まれていたのでidをわたす
		}
	}
	g_Textures[g_TextureCount].filename = filePath;
	return g_TextureCount++;
}

ID3D11ShaderResourceView* Umi::TextureManager::Get(int id) const noexcept
{
	if (id < 0 || id >= g_TextureCount) {
		return NULL;
	}

	return g_Textures[id].shader_resource_view;
}

Umi::TextureManager::~TextureManager()
{
	for (int i = 0; i < g_TextureCount; i++) {
		if (g_Textures[i].shader_resource_view) {
			g_Textures[i].shader_resource_view->Release();
			g_Textures[i].shader_resource_view = NULL;
		}
	}
	g_TextureCount = 0;
}

void Umi::TextureManager::Apply(int id)
{
	ID3D11ShaderResourceView* texture = Get(id);
	g_DeviceContext->PSSetShaderResources(0, 1, &texture);
}

void Umi::TextureManager::LoadMain()
{
	for (size_t i = 0; i < g_TextureCount; i++)
	{
		if (!g_Textures[i].loaded)
		{
			// テクスチャ読み込み
			DirectX::TexMetadata metadata;
			DirectX::ScratchImage image;
			//LoadFromWICFile(filePath.c_str(), WIC_FLAGS_NONE, &metadata, image);
			HRESULT hr = LoadFromWICFile(g_Textures[i].filename.c_str(), WIC_FLAGS_NONE, &metadata, image);
			if (FAILED(hr)) {
				MessageBoxW(NULL, L"Couldn't read a texture file", g_Textures[i].filename.c_str(), MB_ICONEXCLAMATION | MB_OK);
				return;
			}
			CreateShaderResourceView(g_Device, image.GetImages(), image.GetImageCount(), metadata, &g_Textures[i].shader_resource_view);
			//g_Textures[g_TextureCount].width = (int)metadata.width;
			//g_Textures[g_TextureCount].height = (int)metadata.height;

			if (!g_Textures[i].shader_resource_view) {
				MessageBoxW(NULL, L"Couldn't read a texture file", g_Textures[i].filename.c_str(), MB_ICONEXCLAMATION | MB_OK);
				return;
			}

			g_Textures[i].loaded = true;
		}
	}
}