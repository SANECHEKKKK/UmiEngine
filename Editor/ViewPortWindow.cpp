#include "ViewPortWindow.h"
#include <ImGui/imgui.h>

void Umi::ViewPortWindow::Resize(ID3D11Device* device, uint32_t newWidth, uint32_t newHeight)
{
    if (newWidth == 0 || newHeight == 0)
        return;

    if (newWidth == width && newHeight == height)
        return;

    width = newWidth;
    height = newHeight;

    colorTexture.Reset();
    rtv.Reset();
    srv.Reset();

    depthTexture.Reset();
    dsv.Reset();

    D3D11_TEXTURE2D_DESC colorDesc = {};
    colorDesc.Width = width;
    colorDesc.Height = height;
    colorDesc.MipLevels = 1;
    colorDesc.ArraySize = 1;
    colorDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    colorDesc.SampleDesc.Count = 1;
    colorDesc.Usage = D3D11_USAGE_DEFAULT;
    colorDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;

    device->CreateTexture2D(&colorDesc, nullptr, colorTexture.GetAddressOf());
    device->CreateRenderTargetView(colorTexture.Get(), nullptr, rtv.GetAddressOf());
    device->CreateShaderResourceView(colorTexture.Get(), nullptr, srv.GetAddressOf());

    D3D11_TEXTURE2D_DESC depthDesc = {};
    depthDesc.Width = width;
    depthDesc.Height = height;
    depthDesc.MipLevels = 1;
    depthDesc.ArraySize = 1;
    depthDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
    depthDesc.SampleDesc.Count = 1;
    depthDesc.Usage = D3D11_USAGE_DEFAULT;
    depthDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;

    device->CreateTexture2D(&depthDesc, nullptr, depthTexture.GetAddressOf());
    device->CreateDepthStencilView(depthTexture.Get(), nullptr, dsv.GetAddressOf());
}

void Umi::ViewPortWindow::BeginRender(ID3D11DeviceContext* context)
{
    float clearColor[4] = { 0.1f, 0.1f, 0.1f, 1.0f };

    context->OMSetRenderTargets(1, rtv.GetAddressOf(), dsv.Get());
    context->ClearRenderTargetView(rtv.Get(), clearColor);
    context->ClearDepthStencilView(
        dsv.Get(),
        D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL,
        1.0f,
        0
    );

    D3D11_VIEWPORT vp = {};
    vp.TopLeftX = 0.0f;
    vp.TopLeftY = 0.0f;
    vp.Width = static_cast<float>(width);
    vp.Height = static_cast<float>(height);
    vp.MinDepth = 0.0f;
    vp.MaxDepth = 1.0f;

    context->RSSetViewports(1, &vp);
}

void Umi::ViewPortWindow::DrawImGui()
{
    ImGui::Begin("Scene");

    ImVec2 availableSize = ImGui::GetContentRegionAvail();

    uint32_t newWidth = static_cast<uint32_t>(availableSize.x);
    uint32_t newHeight = static_cast<uint32_t>(availableSize.y);

    // Call Resize outside this function if you don't store device here.
    // viewport.Resize(device, newWidth, newHeight);

    ImGui::Image(
        reinterpret_cast<ImTextureID>(srv.Get()),
        availableSize
    );

    ImGui::End();
}
