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

		pDevice->CreateTexture2D(&desc, nullptr, _frontFacesBuffer.ReleaseAndGetAddressOf());
		pDevice->CreateShaderResourceView(_frontFacesBuffer.Get(), nullptr, _frontFacesSRV.ReleaseAndGetAddressOf());
		pDevice->CreateRenderTargetView(_frontFacesBuffer.Get(), nullptr, _frontFacesRTV.ReleaseAndGetAddressOf());

		pDevice->CreateTexture2D(&desc, nullptr, _backFacesBuffer.ReleaseAndGetAddressOf());
		pDevice->CreateShaderResourceView(_backFacesBuffer.Get(), nullptr, _backFacesSRV.ReleaseAndGetAddressOf());
		pDevice->CreateRenderTargetView(_backFacesBuffer.Get(), nullptr, _backFacesRTV.ReleaseAndGetAddressOf());
    }

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
    context.OMSetBlendState(nullptr, nullptr, 0xFFFFFFFFu);

    // draw front faces
    context.RSSetState(_rs.Get());

    context.OMSetRenderTargets(1u, _frontFacesRTV.GetAddressOf(), nullptr);
    context.ClearRenderTargetView(_frontFacesRTV.Get(), clear_color);

	_object->Draw(context);

    // draw back faces
    context.RSSetState(_rsBack.Get());

    context.OMSetRenderTargets(1u, _backFacesRTV.GetAddressOf(), nullptr);
    context.ClearRenderTargetView(_backFacesRTV.Get(), clear_color);

	_object->Draw(context);

    // Set Shader Resources
    context.OMSetRenderTargets(1u, _resultRTV.GetAddressOf(), _dsv.Get());
    static ID3D11ShaderResourceView* srv[3]{ _volumeTexView.Get(), _frontFacesSRV.Get(), _backFacesSRV.Get() };
    context.PSSetShaderResources(1u, 3u, &srv[0]);

    // Draw Volume
    DrawStage::Run(context);
}

void DrawVolumeStage::Consume(ID3D11Resource* resource, int32_t attribute_id)
{
    if (attribute_id == _inputID)
        pDevice->CreateShaderResourceView(resource, nullptr, _previous.ReleaseAndGetAddressOf());
    else if (attribute_id == _volumeTexID)
        pDevice->CreateShaderResourceView(resource, nullptr, _volumeTexView.ReleaseAndGetAddressOf());
}
