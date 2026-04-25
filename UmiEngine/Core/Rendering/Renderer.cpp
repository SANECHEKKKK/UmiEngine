#include <Rendering/RenderSystem.h>

#include <Registry/Registry.h>

#include <Transform/TransformComponent.h>
#include <Texture/TextureComponent.h>
#include <Model/ModelComponent.h>
#include <Animation/Animator3DComponent.h>
#include <Collision/BoxCollider3DComponent.h>
#include <Bar/BarComponent.h>

#include <Texture/TextureManager.h>
#include <Model/ModelManager.h>

#include <EngineContext/EngineContext.h>
#include <Graphics/Graphics.h>
#include <Shaders/Shaders.h>
#include <Settings/Settings.h>
#include <Camera/Camera.h>
#include <Rendering/Renderer.h>
#include <Vertex/Vertex.h>

#include <Text/TextComponent.h>
#include <d2d1.h>
#include <dwrite.h>

using namespace Umi;

//----STRUCTS FOR BUFFERS----//
struct AlphaBufferType
{
	float alpha;
	float padding[3]; // Padding to align to 16 bytes
};

struct MaterialBuffer {
	XMFLOAT4 diffuseColor;
	BOOL hasTexture;
	XMFLOAT3 padding;
};
//--------------------------//

inline DWRITE_TEXT_ALIGNMENT AlignmentToDWriteTextAlignment(Alignment alignment)
{
	switch (alignment)
	{
	case Alignment::Left:
		return DWRITE_TEXT_ALIGNMENT_LEADING;
	case Alignment::Right:
		return DWRITE_TEXT_ALIGNMENT_TRAILING;
	case Alignment::Center:
		return DWRITE_TEXT_ALIGNMENT_CENTER;
	default:
		return DWRITE_TEXT_ALIGNMENT_LEADING;
	}
}

inline DWRITE_PARAGRAPH_ALIGNMENT AlignmentToDWriteParagraphAlignment(Alignment alignment)
{
	switch (alignment)
	{
	case Alignment::Left:
		return DWRITE_PARAGRAPH_ALIGNMENT_NEAR;
		break;
	case Alignment::Center:
		return DWRITE_PARAGRAPH_ALIGNMENT_CENTER;
		break;
	case Alignment::Right:
		return DWRITE_PARAGRAPH_ALIGNMENT_FAR;
		break;
	}
}

inline DWRITE_FONT_WEIGHT FontWeightToDWriteFontWeight(FontWeight fontWeight)
{
	switch (fontWeight)
	{
	case FontWeight::Normal:
		return DWRITE_FONT_WEIGHT_NORMAL;
	case FontWeight::Bold:
		return DWRITE_FONT_WEIGHT_BOLD;
	case FontWeight::Light:
		return DWRITE_FONT_WEIGHT_LIGHT;
	default:
		return DWRITE_FONT_WEIGHT_NORMAL;
	}
}

inline DWRITE_FONT_STYLE FontStyleToDWriteFontStyle(FontStyle fontStyle)
{
	switch (fontStyle)
	{
	case FontStyle::Normal:
		return DWRITE_FONT_STYLE_NORMAL;
	case FontStyle::Italic:
		return DWRITE_FONT_STYLE_ITALIC;
	case FontStyle::Oblique:
		return DWRITE_FONT_STYLE_OBLIQUE;
	default:
		return DWRITE_FONT_STYLE_NORMAL;
	}
}

inline DWRITE_FONT_STRETCH FontStretchToDWriteFontStretch(FontStretch fontStretch)
{
	switch (fontStretch)
	{
	case FontStretch::Condensed:
		return DWRITE_FONT_STRETCH_CONDENSED;
	case FontStretch::Normal:
		return DWRITE_FONT_STRETCH_NORMAL;
	case FontStretch::Expanded:
		return DWRITE_FONT_STRETCH_EXPANDED;
	default:
		return DWRITE_FONT_STRETCH_NORMAL;
	}
}

Renderer::Renderer(Graphics* gr, EngineContext& engineContext) : engineContext(engineContext), graphics(*gr)
{
	D3D11_BUFFER_DESC bd{};
	bd.Usage = D3D11_USAGE_DYNAMIC;
	bd.ByteWidth = sizeof(Vertex2D) * 4;
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	gr->DirectXGetDevice()->CreateBuffer(&bd, NULL, &g_VertexBuffer);


	//D3D11_BUFFER_DESC bd{};
	bd.Usage = D3D11_USAGE_DYNAMIC;
	bd.ByteWidth = sizeof(AlphaBufferType);
	bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

	gr->DirectXGetDevice()->CreateBuffer(&bd, NULL, &alphaBuffer);

	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(MaterialBuffer);
	bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bd.CPUAccessFlags = 0;
	gr->DirectXGetDevice()->CreateBuffer(&bd, NULL, &materialBuffer);

	D3D11_BUFFER_DESC boneBufferDesc{};
	boneBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	boneBufferDesc.ByteWidth = sizeof(XMMATRIX) * 100;
	boneBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	boneBufferDesc.CPUAccessFlags = 0;
	HRESULT hr = gr->DirectXGetDevice()->CreateBuffer(&boneBufferDesc, nullptr, &boneConstantBuffer);
	if (FAILED(hr))
	{
		boneConstantBuffer = nullptr;
		boneBufferBones = 0;
	}
	else
	{
		boneBufferBones = 100;
	}

#ifdef _DEBUG
	bd.Usage = D3D11_USAGE_DYNAMIC;
	bd.ByteWidth = sizeof(Vertex3D) * 24;
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	gr->DirectXGetDevice()->CreateBuffer(&bd, NULL, &g_DebugLineBuffer);
#endif

	shaders2D = std::make_unique<Shaders2D>(gr->DirectXGetDevice(), gr->DirectXGetDeviceContext());
	shaders3D = std::make_unique<Shaders3D>(gr->DirectXGetDevice(), gr->DirectXGetDeviceContext());

	// Blend state
	D3D11_BLEND_DESC blendDesc = {};
	blendDesc.RenderTarget[0].BlendEnable = TRUE;
	blendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
	blendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
	blendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
	blendDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
	blendDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

	ID3D11BlendState* blendState;
	gr->DirectXGetDevice()->CreateBlendState(&blendDesc, &blendState);
	gr->DirectXGetDeviceContext()->OMSetBlendState(blendState, nullptr, 0xffffffff);

	D3D11_RASTERIZER_DESC rd = {};
	rd.FillMode = D3D11_FILL_SOLID;
	rd.CullMode = D3D11_CULL_BACK;
	rd.DepthClipEnable = TRUE;
	rd.MultisampleEnable = FALSE;
	rd.ScissorEnable = FALSE;
	gr->DirectXGetDevice()->CreateRasterizerState(&rd, &g_RasterizerStateDefault);

	rd.ScissorEnable = TRUE;
	gr->DirectXGetDevice()->CreateRasterizerState(&rd, &g_RasterizerStateScissors);

	gr->DirectXGetDeviceContext()->RSSetState(g_RasterizerStateDefault);

	D3D11_DEPTH_STENCIL_DESC dsd = {};
	dsd.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	dsd.DepthFunc = D3D11_COMPARISON_LESS;
	dsd.StencilEnable = FALSE;

	dsd.DepthEnable = FALSE;
	gr->DirectXGetDevice()->CreateDepthStencilState(&dsd, &g_DepthStencilStateDepthDisable);

	dsd.DepthEnable = TRUE;
	gr->DirectXGetDevice()->CreateDepthStencilState(&dsd, &g_DepthStencilStateDepthEnable);

	g_DeviceContext = gr->DirectXGetDeviceContext();
	g_DeviceContext->OMSetDepthStencilState(g_DepthStencilStateDepthDisable, NULL);
}

Renderer::~Renderer()
{
	if (g_VertexBuffer)
	{
		g_VertexBuffer->Release();
		g_VertexBuffer = nullptr;
	}
	if (alphaBuffer)
	{
		alphaBuffer->Release();
		alphaBuffer = nullptr;
	}
#ifdef _DEBUG
	if (g_DebugLineBuffer)
	{
		g_DebugLineBuffer->Release();
		g_DebugLineBuffer = nullptr;
	}
#endif
	SAFE_RELEASE(g_RasterizerStateDefault);
	SAFE_RELEASE(g_RasterizerStateScissors);
	SAFE_RELEASE(currentTextFormat);
	SAFE_RELEASE(textBrush);

};

void Renderer::Render2D(Transform2DComponent& transform, TextureComponent& texture)
{
	SetDepthEnable(false);

	engineContext.textureManager.Apply(texture.id);

	D3D11_MAPPED_SUBRESOURCE msr;
	shaders2D->DirectXGetDeviceContext()->Map(g_VertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &msr);

	Vertex2D* v = (Vertex2D*)msr.pData;

	v[0].position = { -transform.size.x / 2, -transform.size.y / 2 , 0.0f };
	v[1].position = { transform.size.x / 2 , -transform.size.y / 2 , 0.0f };
	v[2].position = { -transform.size.x / 2 , transform.size.y / 2 , 0.0f };
	v[3].position = { transform.size.x / 2 , transform.size.y / 2 , 0.0f };

	v[0].texcoord = { texture.rect.tx,							texture.rect.ty };
	v[1].texcoord = { texture.rect.tx + texture.rect.tw,		texture.rect.ty };
	v[2].texcoord = { texture.rect.tx,							texture.rect.ty + texture.rect.th };
	v[3].texcoord = { texture.rect.tx + texture.rect.tw,		texture.rect.ty + texture.rect.th };

	shaders2D->DirectXGetDeviceContext()->Unmap(g_VertexBuffer, 0);

	shaders2D->Begin();

	XMMATRIX matrix = XMMatrixIdentity();

	//XMMATRIX cameramatrix = mainCamera->GetViewMatrix();

	matrix *= XMMatrixScaling(transform.scale.x, transform.scale.y, 1.0f);

	matrix *= XMMatrixRotationZ(transform.rot);

	matrix *= XMMatrixTranslation(transform.pos.x, transform.pos.y, 0.0f);

	//matrix *= XMMatrixOrthographicOffCenterLH(0.0f, engineContext.settings.getResolution().width, engineContext.settings.getResolution().height, 0.0f, 0.0f, 1.0f);

	if (currentCamera)
		matrix *= currentCamera->GetViewProjectionMatrix();

	//matrix = XMMatrixTranspose(matrix);

	shaders2D->SetMatrix(matrix);

	UINT stride = sizeof(Vertex2D);
	UINT offset = 0;

	shaders2D->DirectXGetDeviceContext()->IASetVertexBuffers(0, 1, &g_VertexBuffer, &stride, &offset);

	shaders2D->DirectXGetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	AlphaBufferType alphaData;
	alphaData.alpha = texture.alpha; // 0.0 = fully transparent, 1.0 = fully opaque

	D3D11_MAPPED_SUBRESOURCE mappedResource;
	shaders2D->DirectXGetDeviceContext()->Map(alphaBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	memcpy(mappedResource.pData, &alphaData, sizeof(AlphaBufferType));
	shaders2D->DirectXGetDeviceContext()->Unmap(alphaBuffer, 0);

	shaders2D->DirectXGetDeviceContext()->PSSetConstantBuffers(0, 1, &alphaBuffer);

	shaders2D->DirectXGetDeviceContext()->Draw(4, 0);
}

void Renderer::RenderUi(TransformUiComponent& transform, TextureComponent& texture)
{
	SetDepthEnable(false);

	engineContext.textureManager.Apply(texture.id);

	D3D11_MAPPED_SUBRESOURCE msr;
	shaders2D->DirectXGetDeviceContext()->Map(g_VertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &msr);

	Vertex2D* v = (Vertex2D*)msr.pData;

	v[0].position = { -transform.size.x / 2, -transform.size.y / 2 , 0.0f };
	v[1].position = { transform.size.x / 2 , -transform.size.y / 2 , 0.0f };
	v[2].position = { -transform.size.x / 2 , transform.size.y / 2 , 0.0f };
	v[3].position = { transform.size.x / 2 , transform.size.y / 2 , 0.0f };

	v[0].texcoord = { texture.rect.tx,							texture.rect.ty };
	v[1].texcoord = { texture.rect.tx + texture.rect.tw,		texture.rect.ty };
	v[2].texcoord = { texture.rect.tx,							texture.rect.ty + texture.rect.th };
	v[3].texcoord = { texture.rect.tx + texture.rect.tw,		texture.rect.ty + texture.rect.th };

	shaders2D->DirectXGetDeviceContext()->Unmap(g_VertexBuffer, 0);

	shaders2D->Begin();

	XMMATRIX matrix = XMMatrixIdentity();

	matrix *= XMMatrixScaling(transform.scale.x, transform.scale.y, 1.0f);

	matrix *= XMMatrixRotationZ(transform.rot);

	matrix *= XMMatrixTranslation(transform.pos.x, transform.pos.y, 0.0f);

	matrix *= XMMatrixOrthographicOffCenterLH(0.0f, engineContext.settings.getResolution().width, engineContext.settings.getResolution().height, 0.0f, 0.0f, 1.0f);

	shaders2D->SetMatrix(matrix);

	UINT stride = sizeof(Vertex2D);
	UINT offset = 0;

	shaders2D->DirectXGetDeviceContext()->IASetVertexBuffers(0, 1, &g_VertexBuffer, &stride, &offset);

	shaders2D->DirectXGetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	AlphaBufferType alphaData;
	alphaData.alpha = texture.alpha; // 0.0 = fully transparent, 1.0 = fully opaque

	D3D11_MAPPED_SUBRESOURCE mappedResource;
	shaders2D->DirectXGetDeviceContext()->Map(alphaBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	memcpy(mappedResource.pData, &alphaData, sizeof(AlphaBufferType));
	shaders2D->DirectXGetDeviceContext()->Unmap(alphaBuffer, 0);

	shaders2D->DirectXGetDeviceContext()->PSSetConstantBuffers(0, 1, &alphaBuffer);

	shaders2D->DirectXGetDeviceContext()->Draw(4, 0);
}

void Renderer::RenderBar(TransformUiComponent& transform, TextureComponent& texture, BarComponent& bar)
{
	float position = (static_cast<float>(bar.currentValue - bar.minValue) / (bar.maxValue - bar.minValue)) * (transform.getRight() - transform.getLeft());
	D3D11_RECT barFillRect = {};
	barFillRect.left = static_cast<LONG>(transform.getLeft());
	barFillRect.top = static_cast<LONG>(transform.getTop());
	barFillRect.right = static_cast<LONG>(transform.getLeft() + position);
	barFillRect.bottom = static_cast<LONG>(transform.getBottom());

	SetDepthEnable(false);

	engineContext.textureManager.Apply(texture.id);

	D3D11_MAPPED_SUBRESOURCE msr;
	shaders2D->DirectXGetDeviceContext()->Map(g_VertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &msr);

	Vertex2D* v = (Vertex2D*)msr.pData;

	v[0].position = { -transform.size.x / 2, -transform.size.y / 2 , 0.0f };
	v[1].position = { transform.size.x / 2 , -transform.size.y / 2 , 0.0f };
	v[2].position = { -transform.size.x / 2 , transform.size.y / 2 , 0.0f };
	v[3].position = { transform.size.x / 2 , transform.size.y / 2 , 0.0f };

	v[0].texcoord = { texture.rect.tx,							texture.rect.ty };
	v[1].texcoord = { texture.rect.tx + texture.rect.tw,		texture.rect.ty };
	v[2].texcoord = { texture.rect.tx,							texture.rect.ty + texture.rect.th };
	v[3].texcoord = { texture.rect.tx + texture.rect.tw,		texture.rect.ty + texture.rect.th };

	shaders2D->DirectXGetDeviceContext()->Unmap(g_VertexBuffer, 0);

	shaders2D->Begin();

	XMMATRIX matrix = XMMatrixIdentity();

	matrix *= XMMatrixScaling(transform.scale.x, transform.scale.y, 1.0f);

	matrix *= XMMatrixRotationZ(transform.rot);

	matrix *= XMMatrixTranslation(transform.pos.x, transform.pos.y, 0.0f);

	matrix *= XMMatrixOrthographicOffCenterLH(0.0f, engineContext.settings.getResolution().width, engineContext.settings.getResolution().height, 0.0f, 0.0f, 1.0f);

	shaders2D->SetMatrix(matrix);

	UINT stride = sizeof(Vertex2D);
	UINT offset = 0;

	shaders2D->DirectXGetDeviceContext()->IASetVertexBuffers(0, 1, &g_VertexBuffer, &stride, &offset);

	shaders2D->DirectXGetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	AlphaBufferType alphaData;
	alphaData.alpha = texture.alpha; // 0.0 = fully transparent, 1.0 = fully opaque

	D3D11_MAPPED_SUBRESOURCE mappedResource;
	shaders2D->DirectXGetDeviceContext()->Map(alphaBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	memcpy(mappedResource.pData, &alphaData, sizeof(AlphaBufferType));
	shaders2D->DirectXGetDeviceContext()->Unmap(alphaBuffer, 0);

	shaders2D->DirectXGetDeviceContext()->PSSetConstantBuffers(0, 1, &alphaBuffer);

	SetScissorEnabled(true);
	g_DeviceContext->RSSetScissorRects(1, &barFillRect);

	shaders2D->DirectXGetDeviceContext()->Draw(4, 0);

	SetScissorEnabled(false);
}

void Renderer::RenderText(TransformUiComponent& transform, TextComponent& text)
{
	// Get fresh pointers each time instead of using stored ones
	auto* id2d1RenderTarget = graphics.GetD2DRenderTarget();
	auto* idWriteFactory = graphics.GetDWriteFactory();
	
	if (!id2d1RenderTarget || !idWriteFactory)
		return;

	if (currentTextFormat)
	{
		SAFE_RELEASE(currentTextFormat);
	}

	HRESULT hr = idWriteFactory->CreateTextFormat(
		text.fontName.c_str(),
		nullptr,
		FontWeightToDWriteFontWeight(text.fontWeight),
		FontStyleToDWriteFontStyle(text.fontStyle),
		FontStretchToDWriteFontStretch(text.fontStretch),
		text.fontSize,
		L"en-us",
		&currentTextFormat
	);

	if (FAILED(hr))
		return;

	currentTextFormat->SetTextAlignment(AlignmentToDWriteTextAlignment(text.horizontalAlignment));
	currentTextFormat->SetParagraphAlignment(AlignmentToDWriteParagraphAlignment(text.verticalAlignment));

	if (!textBrush)
	{
		id2d1RenderTarget->CreateSolidColorBrush(
			D2D1::ColorF(text.color.x, text.color.y, text.color.z, text.color.w),
			&textBrush
		);
	}
	else
	{
		textBrush->SetColor(D2D1::ColorF(text.color.x, text.color.y, text.color.z, text.color.w));
	}

	D2D1_RECT_F layoutRect = D2D1::RectF(
		transform.pos.x - transform.size.x / 2 + text.offsetX,
		transform.pos.y - transform.size.y / 2 + text.offsetY,
		transform.pos.x + transform.size.x / 2 + text.offsetX,
		transform.pos.y + transform.size.y / 2 + text.offsetY
	);

	id2d1RenderTarget->BeginDraw();

	id2d1RenderTarget->DrawText(
		text.text.c_str(),
		static_cast<UINT32>(text.text.length()),
		currentTextFormat,
		layoutRect,
		textBrush
	);

	id2d1RenderTarget->EndDraw();
}

void Renderer::RenderText(Transform3DComponent& transform3d, TextComponent& text)
{
	// Get fresh pointers each time
	auto* id2d1RenderTarget = graphics.GetD2DRenderTarget();
	auto* idWriteFactory = graphics.GetDWriteFactory();
	
	if (!id2d1RenderTarget || !idWriteFactory)
		return;

	if (currentTextFormat)
	{
		SAFE_RELEASE(currentTextFormat);
	}

	Transform2DComponent transform;
	transform.pos = currentCamera->WorldToScreen(transform3d.pos);
	transform.size = { 300.0f, 50.0f };

	HRESULT hr = idWriteFactory->CreateTextFormat(
		text.fontName.c_str(),
		nullptr,
		FontWeightToDWriteFontWeight(text.fontWeight),
		FontStyleToDWriteFontStyle(text.fontStyle),
		FontStretchToDWriteFontStretch(text.fontStretch),
		text.fontSize,
		L"en-us",
		&currentTextFormat
	);

	if (FAILED(hr))
		return;

	currentTextFormat->SetTextAlignment(AlignmentToDWriteTextAlignment(text.horizontalAlignment));
	currentTextFormat->SetParagraphAlignment(AlignmentToDWriteParagraphAlignment(text.verticalAlignment));

	if (!textBrush)
	{
		id2d1RenderTarget->CreateSolidColorBrush(
			D2D1::ColorF(text.color.x, text.color.y, text.color.z, text.color.w),
			&textBrush
		);
	}
	else
	{
		textBrush->SetColor(D2D1::ColorF(text.color.x, text.color.y, text.color.z, text.color.w));
	}

	D2D1_RECT_F layoutRect = D2D1::RectF(
		transform.pos.x,
		transform.pos.y,
		transform.pos.x + transform.size.x,
		transform.pos.y + transform.size.y
	);

	id2d1RenderTarget->BeginDraw();

	id2d1RenderTarget->DrawText(
		text.text.c_str(),
		static_cast<UINT32>(text.text.length()),
		currentTextFormat,
		layoutRect,
		textBrush
	);

	id2d1RenderTarget->EndDraw();
}

void Renderer::Render3D(Transform3DComponent& transform, ModelComponent& model)
{
	ClearRenderState();

	SetDepthEnable(true);

	shaders3D->Begin();

	MatrixBuffer matrices;

	XMMATRIX translation = XMMatrixTranslation(transform.pos.x, transform.pos.y, transform.pos.z);
	XMMATRIX rotationMatrix = XMMatrixRotationRollPitchYaw(transform.rot.x, transform.rot.y, transform.rot.z);
	XMMATRIX scaling = XMMatrixScaling(transform.scale.x, transform.scale.y, transform.scale.z);

	matrices.world = XMMatrixTranspose(scaling * rotationMatrix * translation);
	matrices.view = XMMatrixTranspose(currentCamera->GetViewMatrix());
	matrices.projection = XMMatrixTranspose(currentCamera->GetProjectionMatrix());

	g_DeviceContext->UpdateSubresource(shaders3D->matrixBuffer, 0, nullptr, &matrices, 0, 0);
	g_DeviceContext->VSSetConstantBuffers(0, 1, &shaders3D->matrixBuffer);

	ID3D11Buffer* nullBuffer = nullptr;
	g_DeviceContext->PSSetConstantBuffers(0, 1, &nullBuffer);

	g_DeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	for (auto& mesh : engineContext.modelManager.GetMesh(model.modelId))
	{
		UINT stride = sizeof(Vertex3D);
		UINT offset = 0;

		g_DeviceContext->IASetVertexBuffers(0, 1, mesh.vertexBuffer.GetAddressOf(), &stride, &offset);
		g_DeviceContext->IASetIndexBuffer(mesh.indexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);

		MaterialBuffer matBuffer;
		matBuffer.diffuseColor = mesh.diffuseColor;
		matBuffer.hasTexture = mesh.hasTexture ? 1 : 0;

		g_DeviceContext->UpdateSubresource(materialBuffer, 0, nullptr, &matBuffer, 0, 0);
		g_DeviceContext->PSSetConstantBuffers(1, 1, &materialBuffer);

		if (mesh.hasTexture)
		{
			g_DeviceContext->PSSetShaderResources(0, 1, mesh.texture.GetAddressOf());
		}
		else
		{
			ID3D11ShaderResourceView* nullSRV = nullptr;
			g_DeviceContext->PSSetShaderResources(0, 1, &nullSRV);
		}

		ID3D11SamplerState* sampler = shaders3D->DirectXGetSamplerState();
		g_DeviceContext->PSSetSamplers(0, 1, &sampler);

		g_DeviceContext->DrawIndexed(mesh.indexCount, 0, 0);
	}
}

void Renderer::Render3DAnimated(Transform3DComponent& transform, ModelComponent& model, Animator3DComponent& animator)
{
	ClearRenderState();

	SetDepthEnable(true);

	shaders3D->Begin();

	std::vector<XMMATRIX> boneMatrices = animator.finalBoneMatrices;

	if (boneMatrices.empty())
	{
		boneMatrices.push_back(XMMatrixIdentity());
	}

	if (boneMatrices.size() < boneBufferBones)
	{
		boneMatrices.resize(boneBufferBones, XMMatrixIdentity());
	}

	for (auto& mat : boneMatrices)
	{
		mat = XMMatrixTranspose(mat);
	}

	g_DeviceContext->UpdateSubresource(boneConstantBuffer, 0, nullptr, boneMatrices.data(), 0, 0);
	g_DeviceContext->VSSetConstantBuffers(2, 1, &boneConstantBuffer);

	MatrixBuffer matrices;

	XMMATRIX translation = XMMatrixTranslation(transform.pos.x, transform.pos.y, transform.pos.z);
	XMMATRIX rotationMatrix = XMMatrixRotationRollPitchYaw(transform.rot.x, transform.rot.y, transform.rot.z);
	XMMATRIX scaling = XMMatrixScaling(transform.scale.x, transform.scale.y, transform.scale.z);

	matrices.world = XMMatrixTranspose(scaling * rotationMatrix * translation);
	matrices.view = XMMatrixTranspose(currentCamera->GetViewMatrix());
	matrices.projection = XMMatrixTranspose(currentCamera->GetProjectionMatrix());

	g_DeviceContext->UpdateSubresource(shaders3D->matrixBuffer, 0, nullptr, &matrices, 0, 0);
	g_DeviceContext->VSSetConstantBuffers(0, 1, &shaders3D->matrixBuffer);

	ID3D11Buffer* nullBuffer = nullptr;
	g_DeviceContext->PSSetConstantBuffers(0, 1, &nullBuffer);


	g_DeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	for (auto& mesh : engineContext.modelManager.GetMesh(model.modelId))
	{
		UINT stride = sizeof(Vertex3D);
		UINT offset = 0;

		g_DeviceContext->IASetVertexBuffers(0, 1, mesh.vertexBuffer.GetAddressOf(), &stride, &offset);
		g_DeviceContext->IASetIndexBuffer(mesh.indexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);

		MaterialBuffer matBuffer;
		matBuffer.diffuseColor = mesh.diffuseColor;
		matBuffer.hasTexture = mesh.hasTexture ? 1 : 0;

		g_DeviceContext->UpdateSubresource(materialBuffer, 0, nullptr, &matBuffer, 0, 0);
		g_DeviceContext->PSSetConstantBuffers(1, 1, &materialBuffer);

		if (mesh.hasTexture)
		{
			g_DeviceContext->PSSetShaderResources(0, 1, mesh.texture.GetAddressOf());
		}
		else
		{
			ID3D11ShaderResourceView* nullSRV = nullptr;
			g_DeviceContext->PSSetShaderResources(0, 1, &nullSRV);
		}

		ID3D11SamplerState* sampler = shaders3D->DirectXGetSamplerState();
		g_DeviceContext->PSSetSamplers(0, 1, &sampler);

		g_DeviceContext->DrawIndexed(mesh.indexCount, 0, 0);
	}
}
#ifdef _DEBUG
void Renderer::RenderBoxCollider(BoxCollider3DComponent& boxCollider)
{
	ClearRenderState();
	SetDepthEnable(false);

	shaders3D->Begin();

	XMFLOAT3 halfSize(boxCollider.size.x * 0.5f, boxCollider.size.y * 0.5f, boxCollider.size.z * 0.5f);
	XMFLOAT3 center(boxCollider.pos.x, boxCollider.pos.y, boxCollider.pos.z);

	XMFLOAT3 corners[8] = {
		{ center.x - halfSize.x, center.y - halfSize.y, center.z - halfSize.z },
		{ center.x + halfSize.x, center.y - halfSize.y, center.z - halfSize.z },
		{ center.x + halfSize.x, center.y + halfSize.y, center.z - halfSize.z },
		{ center.x - halfSize.x, center.y + halfSize.y, center.z - halfSize.z },
		{ center.x - halfSize.x, center.y - halfSize.y, center.z + halfSize.z },
		{ center.x + halfSize.x, center.y - halfSize.y, center.z + halfSize.z },
		{ center.x + halfSize.x, center.y + halfSize.y, center.z + halfSize.z },
		{ center.x - halfSize.x, center.y + halfSize.y, center.z + halfSize.z }
	};

	D3D11_MAPPED_SUBRESOURCE msr;
	g_DeviceContext->Map(g_DebugLineBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &msr);

	Vertex3D* v = (Vertex3D*)msr.pData;
	XMFLOAT3 normal(0.0f, 0.0f, 0.0f);
	XMFLOAT2 texcoord(0.0f, 0.0f);

	v[0] = { corners[0], normal, texcoord }; v[1] = { corners[1], normal, texcoord };
	v[2] = { corners[1], normal, texcoord }; v[3] = { corners[2], normal, texcoord };
	v[4] = { corners[2], normal, texcoord }; v[5] = { corners[3], normal, texcoord };
	v[6] = { corners[3], normal, texcoord }; v[7] = { corners[0], normal, texcoord };

	v[8] = { corners[4], normal, texcoord };  v[9] = { corners[5], normal, texcoord };
	v[10] = { corners[5], normal, texcoord }; v[11] = { corners[6], normal, texcoord };
	v[12] = { corners[6], normal, texcoord }; v[13] = { corners[7], normal, texcoord };
	v[14] = { corners[7], normal, texcoord }; v[15] = { corners[4], normal, texcoord };

	v[16] = { corners[0], normal, texcoord }; v[17] = { corners[4], normal, texcoord };
	v[18] = { corners[1], normal, texcoord }; v[19] = { corners[5], normal, texcoord };
	v[20] = { corners[2], normal, texcoord }; v[21] = { corners[6], normal, texcoord };
	v[22] = { corners[3], normal, texcoord }; v[23] = { corners[7], normal, texcoord };

	g_DeviceContext->Unmap(g_DebugLineBuffer, 0);

	MatrixBuffer matrices;
	matrices.world = XMMatrixTranspose(XMMatrixIdentity());
	matrices.view = XMMatrixTranspose(currentCamera->GetViewMatrix());
	matrices.projection = XMMatrixTranspose(currentCamera->GetProjectionMatrix());

	g_DeviceContext->UpdateSubresource(shaders3D->matrixBuffer, 0, nullptr, &matrices, 0, 0);
	g_DeviceContext->VSSetConstantBuffers(0, 1, &shaders3D->matrixBuffer);

	MaterialBuffer matBuffer;
	matBuffer.diffuseColor = XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f);
	matBuffer.hasTexture = FALSE;

	g_DeviceContext->UpdateSubresource(materialBuffer, 0, nullptr, &matBuffer, 0, 0);
	g_DeviceContext->PSSetConstantBuffers(1, 1, &materialBuffer);

	ID3D11ShaderResourceView* nullSRV = nullptr;
	g_DeviceContext->PSSetShaderResources(0, 1, &nullSRV);

	UINT stride = sizeof(Vertex3D);
	UINT offset = 0;
	g_DeviceContext->IASetVertexBuffers(0, 1, &g_DebugLineBuffer, &stride, &offset);
	g_DeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINELIST);

	g_DeviceContext->Draw(24, 0);

	SetDepthEnable(true);
}
#endif // _DEBUG

void Renderer::SetCamera(Camera* camera) noexcept
{
	currentCamera = camera;
}

void Renderer::ClearRenderState()
{
	ID3D11ShaderResourceView* nullSRV[8] = { nullptr };
	g_DeviceContext->PSSetShaderResources(0, 8, nullSRV);

	ID3D11Buffer* nullBuffer[4] = { nullptr };
	g_DeviceContext->PSSetConstantBuffers(0, 4, nullBuffer);
	g_DeviceContext->VSSetConstantBuffers(0, 4, nullBuffer);
}

void Renderer::SetDepthEnable(bool enable) noexcept
{
	if (enable)
	{
		g_DeviceContext->OMSetDepthStencilState(g_DepthStencilStateDepthEnable, NULL);
	}
	else
	{
		g_DeviceContext->OMSetDepthStencilState(g_DepthStencilStateDepthDisable, NULL);
	}
}

void Renderer::SetScissorEnabled(bool enabled)
{
	if (enabled)
	{
		g_DeviceContext->RSSetState(g_RasterizerStateScissors);
	}
	else
	{
		g_DeviceContext->RSSetState(g_RasterizerStateDefault);
	}
}