#pragma once
#include <memory>
#include <Windows.h>

struct ID3D11ShaderResourceView;
struct ID3D11Device;
struct ID3D11DeviceContext;
struct ID3D11DepthStencilState;
struct ID3D11Buffer;
struct ID3D11RasterizerState;
class Graphics;

struct ID2D1RenderTarget;
struct IDWriteFactory;
struct IDWriteTextFormat;
struct ID2D1SolidColorBrush;

namespace Umi
{
	struct Transform2DComponent;
	struct TextureComponent;
	struct EngineContext;
	class Shaders2D;
	class Shaders3D;
	class Camera;

	class Renderer
	{
	private:
		Graphics& graphics;

		std::unique_ptr<Shaders2D> shaders2D;
		std::unique_ptr<Shaders3D> shaders3D;

		ID3D11DepthStencilState* g_DepthStencilStateDepthDisable = nullptr;
		ID3D11DepthStencilState* g_DepthStencilStateDepthEnable = nullptr;

		ID3D11DeviceContext* g_DeviceContext = nullptr;

		ID3D11Buffer* g_VertexBuffer = nullptr;
		ID3D11Buffer* alphaBuffer = nullptr;
		ID3D11Buffer* materialBuffer = nullptr;
		ID3D11Buffer* boneConstantBuffer = nullptr;
#ifdef _DEBUG
		ID3D11Buffer* g_DebugLineBuffer = nullptr;
#endif

		ID3D11RasterizerState* g_RasterizerStateDefault = nullptr;
		ID3D11RasterizerState* g_RasterizerStateScissors = nullptr;

		IDWriteTextFormat* currentTextFormat = nullptr;
		ID2D1SolidColorBrush* textBrush = nullptr;

		//std::unique_ptr<class TextManager> textManager;

		UINT boneBufferBones = 0;

		Camera* currentCamera = nullptr;

		EngineContext& engineContext;

	public:
		//--------------2D RENDERING----------------//
		void Render2D(Transform2DComponent& transform, TextureComponent& texture);
		void RenderUi(struct TransformUiComponent& transform, TextureComponent& texture);
		void RenderBar(TransformUiComponent& transform, TextureComponent& texture, struct BarComponent& bar);
		void RenderUiCut();
		void RenderText(struct TransformUiComponent& transform, struct TextComponent& text);
		void RenderText(struct Transform3DComponent& transform, struct TextComponent& text);
		//------------------------------------------//

		//--------------3D RENDERING----------------//
		void Render3D(struct Transform3DComponent& transform, struct ModelComponent& model);
		void Render3DAnimated(Transform3DComponent& transform, ModelComponent& model, struct Animator3DComponent& animator);
#ifdef _DEBUG
		void RenderBoxCollider(struct BoxCollider3DComponent& boxCollider);
#endif // _DEBUG
		//------------------------------------------//

		void SetCamera(Camera* camera) noexcept;

		void ClearRenderState();

		void SetDepthEnable(bool enable) noexcept;

		void SetScissorEnabled(bool enabled);

		void OnResolutionChange(float width, float height) noexcept;

		void Render();

		//void Init(Graphics* gr);

		Renderer(Graphics* gr, EngineContext& engineContext);
		~Renderer();
	};
}