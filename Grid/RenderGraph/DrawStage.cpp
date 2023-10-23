#include "pch.h"
#include "DrawStage.h"

#include "imgui.h"
#include "imnodes.h"
#include "RenderObject.h"

#include "NodeManager.h"

DrawStage::DrawStage(char const* name, std::shared_ptr<RenderObject> object)
	: _name{name}, _object{object}
{
    _incoming[NodeManager::IssueIncomingAttrID()] = -1;
    _attrNames[NodeManager::LastIncomingAttrID()] = { "Before" };

    _resultID = NodeManager::IssueOutgoingAttrID();
    _outgoing[_resultID] = -1;
    _attrNames[_resultID] = { "Result" };

	auto desc {CD3D11_TEXTURE2D_DESC{}};
	desc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
	desc.Usage = D3D11_USAGE_DEFAULT;
	desc.CPUAccessFlags = 0u;
	desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	desc.Width = gViewportInfo.width;
	desc.Height = gViewportInfo.height;
	desc.MipLevels = 1u;
	desc.ArraySize = 1u;
	desc.SampleDesc.Count = 1;
	desc.SampleDesc.Quality = 0;

    pDevice->CreateTexture2D(&desc, nullptr, _resource.ReleaseAndGetAddressOf());
    pDevice->CreateShaderResourceView(_resource.Get(), nullptr, _srv.ReleaseAndGetAddressOf());
    pDevice->CreateRenderTargetView(_resource.Get(), nullptr, _rtv.ReleaseAndGetAddressOf());

    _vp.Width = static_cast<float>(gViewportInfo.width);
    _vp.Height = static_cast<float>(gViewportInfo.height);
    _vp.MinDepth = 0.0f;
    _vp.MaxDepth = 1.0f;
    _vp.TopLeftX = 0.0f;
    _vp.TopLeftY = 0.0f;

    InitRS();
    InitDS();
    InitBS();
}

void DrawStage::Run(ID3D11DeviceContext& context)
{
    context.RSSetViewports(1u, &_vp);
    context.RSSetState(_rs.Get());
    context.OMSetDepthStencilState(_ds.Get(), 0u);
    //context.OMSetBlendState(_bs.Get(), nullptr, 0xFFFFFFFFu);
    context.OMSetBlendState(nullptr, nullptr, 0xFFFFFFFFu);

    context.OMSetRenderTargets(1u, _rtv.GetAddressOf(), _dsv.Get());
    context.ClearRenderTargetView(_rtv.Get(), clear_color);
    context.ClearDepthStencilView(_dsv.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0u);

	_object->Draw(context);
}

void DrawStage::Update(ID3D11DeviceContext& context)
{
    _object->Update(context, 0.0f);
}

void DrawStage::RenderNode() const {
    ImNodes::BeginNode(_id);
    ImNodes::BeginNodeTitleBar();
    ImGui::Text(_stageName.c_str());
    ImGui::Text(_name.c_str());
    ImGui::Text("%d", _id);
    ImNodes::EndNodeTitleBar();

    for (auto& in : _incoming) {
		ImNodes::BeginInputAttribute(in.first);
		ImGui::Text("%s (%d)", _attrNames.at(in.first).c_str(), in.first);
		ImNodes::EndInputAttribute();
    }

    for (auto& out : _outgoing) {
        ImNodes::BeginOutputAttribute(out.first);
        ImGui::Text("%s (%d)", _attrNames.at(out.first).c_str(), out.first);
        ImNodes::EndOutputAttribute();
    }

    ImNodes::EndNode();
}

void DrawStage::Consume(ID3D11Resource* resource, int32_t attribute_id)
{
}

ID3D11Resource* DrawStage::Expose(int32_t attribute_id)
{
    if (attribute_id == _resultID)
        return static_cast<ID3D11Resource*>(_resource.Get());
    return nullptr;
}

void DrawStage::InitRS()
{
    D3D11_RASTERIZER_DESC rd{};

    // Default Solid
    {
        rd.FillMode = D3D11_FILL_MODE::D3D11_FILL_SOLID;
        rd.CullMode = D3D11_CULL_MODE::D3D11_CULL_NONE;
        rd.FrontCounterClockwise = false;
        rd.DepthClipEnable = true;
        rd.MultisampleEnable = false;

        pDevice->CreateRasterizerState(&rd, _rs.ReleaseAndGetAddressOf());
    }
}

void DrawStage::InitDS()
{
    // default
    {
        D3D11_DEPTH_STENCIL_DESC desc{ CD3D11_DEPTH_STENCIL_DESC{CD3D11_DEFAULT{}} };

        pDevice->CreateDepthStencilState(&desc, _ds.ReleaseAndGetAddressOf());
    }

    D3D11_TEXTURE2D_DESC descDepth{};
    descDepth.Usage = D3D11_USAGE_DEFAULT;
    descDepth.BindFlags = D3D11_BIND_DEPTH_STENCIL;
    descDepth.Width = gViewportInfo.width;
    descDepth.Height = gViewportInfo.height;
    descDepth.MipLevels = 1u;
    descDepth.ArraySize = 1u;
    descDepth.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
    descDepth.SampleDesc.Count = 1;
    descDepth.SampleDesc.Quality = 0;

    Microsoft::WRL::ComPtr<ID3D11Texture2D> _ds;
    pDevice->CreateTexture2D(&descDepth, nullptr, _ds.ReleaseAndGetAddressOf());

    D3D11_DEPTH_STENCIL_VIEW_DESC desc_dsv{};
    desc_dsv.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
    desc_dsv.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
    desc_dsv.Texture2D.MipSlice = 0u;
    pDevice->CreateDepthStencilView(_ds.Get(), &desc_dsv, _dsv.ReleaseAndGetAddressOf());
}

void DrawStage::InitBS()
{
    // Default
    {
        D3D11_BLEND_DESC desc{};
        desc.AlphaToCoverageEnable = true;
        desc.IndependentBlendEnable = false;

        desc.RenderTarget[0].BlendEnable = true;
        desc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
        desc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
        desc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;

        desc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ZERO;
        desc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
        desc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;

        desc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

        pDevice->CreateBlendState(&desc, _bs.ReleaseAndGetAddressOf());
    }
}
