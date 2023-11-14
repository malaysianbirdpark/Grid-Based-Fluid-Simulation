#include "pch.h"
#include "DrawVolumeStage.h"

#include "Transform.h"
#include "PipelineStateObject.h"
#include "VolumeCube.h"

#include "NodeManager.h"

#include <cstdlib>
#include <ctime>

#include "fp16.h"

DrawVolumeStage::DrawVolumeStage(ID3D11DeviceContext& context)
    : DrawStage{"Volume Cube", std::move(std::make_shared<VolumeCube>(context))}
{
    _volumeTexID = NodeManager::IssueIncomingAttrID();
    _incoming[_volumeTexID] = -1;
    _attrNames[_volumeTexID] = { "Volume Texture" };

    {
		Microsoft::WRL::ComPtr<ID3D11Texture2D> _frontFacesUvwBuffer;
		Microsoft::WRL::ComPtr<ID3D11Texture2D> _frontFacesWorldBuffer;
		Microsoft::WRL::ComPtr<ID3D11Texture2D> _backFacesUvwBuffer;
		Microsoft::WRL::ComPtr<ID3D11Texture2D> _backFacesWorldBuffer;
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

		pDevice->CreateTexture2D(&desc, nullptr, _frontFacesWorldBuffer.ReleaseAndGetAddressOf());
		pDevice->CreateShaderResourceView(_frontFacesWorldBuffer.Get(), nullptr, _frontFacesWorldSRV.ReleaseAndGetAddressOf());
		pDevice->CreateRenderTargetView(_frontFacesWorldBuffer.Get(), nullptr, _frontFacesWorldRTV.ReleaseAndGetAddressOf());

		pDevice->CreateTexture2D(&desc, nullptr, _backFacesUvwBuffer.ReleaseAndGetAddressOf());
		pDevice->CreateShaderResourceView(_backFacesUvwBuffer.Get(), nullptr, _backFacesUvwSRV.ReleaseAndGetAddressOf());
		pDevice->CreateRenderTargetView(_backFacesUvwBuffer.Get(), nullptr, _backFacesUvwRTV.ReleaseAndGetAddressOf());

		pDevice->CreateTexture2D(&desc, nullptr, _backFacesWorldBuffer.ReleaseAndGetAddressOf());
		pDevice->CreateShaderResourceView(_backFacesWorldBuffer.Get(), nullptr, _backFacesWorldSRV.ReleaseAndGetAddressOf());
		pDevice->CreateRenderTargetView(_backFacesWorldBuffer.Get(), nullptr, _backFacesWorldRTV.ReleaseAndGetAddressOf());
    }

    // jitter
    {
        struct float16_t {
            explicit float16_t() = default;

            explicit float16_t(float x, float y, float z, float w) {
                data[0] = fp16_ieee_from_fp32_value(x);
                data[1] = fp16_ieee_from_fp32_value(y);
                data[2] = fp16_ieee_from_fp32_value(z);
                data[3] = fp16_ieee_from_fp32_value(w);
            }

            uint16_t data[4]{};
        };

		std::srand(std::time(nullptr));
        auto jitter{ std::vector<float16_t>(16) };
        for (auto& i : jitter) {
            i = float16_t{
                (static_cast<float>(std::rand()) / RAND_MAX - 0.5f) / gSimulationInfo.width / 2.0f,
                (static_cast<float>(std::rand()) / RAND_MAX - 0.5f) / gSimulationInfo.height / 2.0f,
                (static_cast<float>(std::rand()) / RAND_MAX - 0.5f) / gSimulationInfo.depth / 2.0f,
                0.0f
            };
        }
        
		Microsoft::WRL::ComPtr<ID3D11Texture1D> _jitterBuffer;
        auto desc{ CD3D11_TEXTURE1D_DESC{} };
        desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
        desc.Usage = D3D11_USAGE_IMMUTABLE;
        desc.Format = DXGI_FORMAT_R16G16B16A16_FLOAT;
        desc.Width = 16;
        desc.MipLevels = 1u;
        desc.CPUAccessFlags = 0u;
        desc.MiscFlags = 0u;
        desc.ArraySize = 1u;

        D3D11_SUBRESOURCE_DATA sd{};
        sd.pSysMem = jitter.data();
        sd.SysMemPitch = 0u;
        sd.SysMemSlicePitch = 0u;

		pDevice->CreateTexture1D(&desc, &sd, _jitterBuffer.ReleaseAndGetAddressOf());
		pDevice->CreateShaderResourceView(_jitterBuffer.Get(), nullptr, _jitterSRV.ReleaseAndGetAddressOf());
    }

    InitRS();
    InitDS();
    InitBS();

    _pso.push_back(std::move(std::make_unique<PipelineStateObject>()));
	_pso.back()->SetVertexShader("./CSO/VolumeCube_VS.cso");
	_pso.back()->SetInputLayout(_object->GetInputElementDest());
	//_pso.back()->SetPixelShader("./CSO/VolumeCube_PS.cso");
	_pso.back()->SetPixelShader("./CSO/VolumeFlame_PS.cso");

    _pso.push_back(std::move(std::make_unique<PipelineStateObject>()));
	_pso.back()->SetVertexShader("./CSO/VolumeCube_VS.cso");
	_pso.back()->SetInputLayout(_object->GetInputElementDest());
	_pso.back()->SetPixelShader("./CSO/DrawVolumeFaces_PS.cso");
}

void DrawVolumeStage::Run(ID3D11DeviceContext& context)
{
    context.RSSetViewports(1u, &_vp);
    context.OMSetDepthStencilState(_ds.Get(), 0u);
    //context.OMSetBlendState(nullptr, nullptr, 0xFFFFFFFFu);

    // draw front faces
    _pso[1]->Bind(context);
    context.RSSetState(_rs.Get());

    ID3D11RenderTargetView* const rtvs0[2]{ _frontFacesUvwRTV.Get(), _frontFacesWorldRTV.Get() };
    context.OMSetRenderTargets(2u, rtvs0, nullptr);
    context.ClearRenderTargetView(_frontFacesUvwRTV.Get(), clear_color);
    context.ClearRenderTargetView(_frontFacesWorldRTV.Get(), clear_color);
    //context.ClearDepthStencilView(_dsv.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0u);

	_object->Draw(context);

    // draw back faces
    context.RSSetState(_rsBack.Get());

    ID3D11RenderTargetView* const rtvs1[2]{ _backFacesUvwRTV.Get(), _backFacesWorldRTV.Get() };
    context.OMSetRenderTargets(2u, rtvs1, nullptr);
    context.ClearRenderTargetView(_backFacesUvwRTV.Get(), clear_color);
    context.ClearRenderTargetView(_backFacesWorldRTV.Get(), clear_color);
    //context.ClearDepthStencilView(_dsv.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0u);

	_object->Draw(context);

    // Set Shader Resources
    context.OMSetRenderTargets(1u, _resultRTV.GetAddressOf(), _gDSV.Get());
    static ID3D11ShaderResourceView* const srv[6]{ _volumeTexView.Get(), _frontFacesUvwSRV.Get(), _frontFacesWorldSRV.Get(), _backFacesUvwSRV.Get(), _backFacesWorldSRV.Get(), _jitterSRV.Get() };
    context.PSSetShaderResources(0u, 6u, srv);

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

void DrawVolumeStage::InitRS()
{
    DrawStage::InitRS();

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
