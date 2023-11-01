#include "pch.h"
#include "DrawVolumeStage.h"

#include "Transform.h"
#include "PipelineStateObject.h"
#include "VolumeCube.h"

#include "NodeManager.h"

DrawVolumeStage::DrawVolumeStage(ID3D11DeviceContext& context)
    : DrawStage{"Volume Cube", std::move(std::make_shared<VolumeCube>(context))}
{
    _volumeTexID = NodeManager::IssueIncomingAttrID();
    _incoming[_volumeTexID] = -1;
    _attrNames[_volumeTexID] = { "Volume Texture" };

    // draw back face
    {
		D3D11_RASTERIZER_DESC rd{};

        rd.FillMode = D3D11_FILL_MODE::D3D11_FILL_SOLID;
        rd.CullMode = D3D11_CULL_MODE::D3D11_CULL_FRONT;
        rd.FrontCounterClockwise = false;
        rd.DepthClipEnable = true;
        rd.MultisampleEnable = false;

        pDevice->CreateRasterizerState(&rd, _rsBack.ReleaseAndGetAddressOf());
    }
    
    {
		auto desc {CD3D11_TEXTURE2D_DESC{}};
		desc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
		desc.Usage = D3D11_USAGE_DEFAULT;
		desc.CPUAccessFlags = 0u;
		desc.Format = DXGI_FORMAT_R16G16B16A16_FLOAT;
		desc.Width = gViewportInfo.width;
		desc.Height = gViewportInfo.height;
		desc.MipLevels = 1u;
		desc.ArraySize = 1u;
		desc.SampleDesc.Count = 1;
		desc.SampleDesc.Quality = 0;

		pDevice->CreateTexture2D(&desc, nullptr, _frontFacesUvwBuffer.ReleaseAndGetAddressOf());
		pDevice->CreateShaderResourceView(_frontFacesUvwBuffer.Get(), nullptr, _frontFacesUvwSRV.ReleaseAndGetAddressOf());
		pDevice->CreateRenderTargetView(_frontFacesUvwBuffer.Get(), nullptr, _frontFacesUvwRTV.ReleaseAndGetAddressOf());

		pDevice->CreateTexture2D(&desc, nullptr, _backFacesUvwBuffer.ReleaseAndGetAddressOf());
		pDevice->CreateShaderResourceView(_backFacesUvwBuffer.Get(), nullptr, _backFacesUvwSRV.ReleaseAndGetAddressOf());
		pDevice->CreateRenderTargetView(_backFacesUvwBuffer.Get(), nullptr, _backFacesUvwRTV.ReleaseAndGetAddressOf());
    }

    InitRS();
    InitDS();
    InitBS();

    _pso.push_back(std::move(std::make_unique<PipelineStateObject>()));
	_pso.back()->SetVertexShader("./CSO/VolumeCube_VS.cso");
	_pso.back()->SetInputLayout(_object->GetInputElementDest());
	_pso.back()->SetPixelShader("./CSO/VolumeCube_PS.cso");

    _pso.push_back(std::move(std::make_unique<PipelineStateObject>()));
	_pso.back()->SetVertexShader("./CSO/VolumeCube_VS.cso");
	_pso.back()->SetInputLayout(_object->GetInputElementDest());
	_pso.back()->SetPixelShader("./CSO/VolumeCubeFaces_PS.cso");
}

void DrawVolumeStage::Run(ID3D11DeviceContext& context)
{
    _pso.back()->Bind(context);

    context.RSSetViewports(1u, &_vp);
    context.OMSetDepthStencilState(_ds.Get(), 0u);
    //context.OMSetBlendState(nullptr, nullptr, 0xFFFFFFFFu);

    // draw front faces
    context.RSSetState(_rs.Get());

    context.OMSetRenderTargets(1u, _frontFacesUvwRTV.GetAddressOf(), nullptr);
    context.ClearRenderTargetView(_frontFacesUvwRTV.Get(), clear_color);
    //context.ClearDepthStencilView(_dsv.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0u);

	_object->Draw(context);

    // draw back faces
    context.RSSetState(_rsBack.Get());

    context.OMSetRenderTargets(1u, _backFacesUvwRTV.GetAddressOf(), nullptr);
    context.ClearRenderTargetView(_backFacesUvwRTV.Get(), clear_color);
    //context.ClearDepthStencilView(_dsv.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0u);

	_object->Draw(context);

    // Set Shader Resources
    context.OMSetRenderTargets(1u, _resultRTV.GetAddressOf(), _gDSV.Get());
    static ID3D11ShaderResourceView* srv[3]{ _volumeTexView.Get(), _frontFacesUvwSRV.Get(), _backFacesUvwSRV.Get() };
    srv[0] = _volumeTexView.Get();
    context.PSSetShaderResources(0u, 3u, &srv[0]);

    // Draw Volume
    context.RSSetViewports(1u, &_vp);
    context.RSSetState(_rs.Get());
    context.OMSetDepthStencilState(_gDS.Get(), 0u);
    context.OMSetBlendState(_bsAlpha.Get(), nullptr, 0xFFFFFFFFu);
    //context.OMSetBlendState(nullptr, nullptr, 0xFFFFFFFFu);

    context.ClearRenderTargetView(_resultRTV.Get(), clear_color);

    // Copy Previous Frame
    if (_previous.Get())
    {
        ID3D11Resource* previous{ nullptr };
        _previous->GetResource(&previous);

        ID3D11Resource* cur{ nullptr };
        _resultRTV->GetResource(&cur);

		context.CopyResource(cur, previous);
    }

    _pso.front()->Bind(context);

	_object->Draw(context);

    static ID3D11ShaderResourceView* null_srv[1] {nullptr};
    context.PSSetShaderResources(0u, 1u, null_srv);
}

void DrawVolumeStage::Consume(ID3D11Resource* resource, int32_t attribute_id)
{
    if (attribute_id == _inputID)
        pDevice->CreateShaderResourceView(resource, nullptr, _previous.ReleaseAndGetAddressOf());
    else if (attribute_id == _volumeTexID)
        pDevice->CreateShaderResourceView(resource, nullptr, _volumeTexView.ReleaseAndGetAddressOf());
}

void DrawVolumeStage::InitBS()
{
    // Alpha Blending
    {
        D3D11_BLEND_DESC desc{ CD3D11_BLEND_DESC{} };
        desc.AlphaToCoverageEnable = false;
        desc.IndependentBlendEnable = false;

        desc.RenderTarget[0].BlendEnable = true;
        desc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
        desc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
        desc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;

        desc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_SRC_ALPHA;
        desc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_INV_SRC_ALPHA;
        desc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;

        desc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

        pDevice->CreateBlendState(&desc, _bsAlpha.ReleaseAndGetAddressOf());
    }

}
