#include "pch.h"
#include "Advection2DStage.h"

#include <d3dcompiler.h>

#include "imnodes.h"
#include "NodeManager.h"

Advection2DStage::Advection2DStage()
    : Compute2DStage{ "2D-Fluid Advection", "./CSO/Advection2D_CS.cso", 32, 32, 1 }
{
    _uav.resize(2);
    _srv.resize(4);
    _nullUav.resize(5);
    _nullSrv.resize(5);

    _resource.resize(2);

	auto desc {CD3D11_TEXTURE2D_DESC{}};
	desc.BindFlags = D3D11_BIND_UNORDERED_ACCESS | D3D11_BIND_SHADER_RESOURCE;
	desc.Usage = D3D11_USAGE_DEFAULT;
	desc.CPUAccessFlags = 0u;
	//desc.Format = DXGI_FORMAT_R16G16_FLOAT;
	desc.Format = DXGI_FORMAT_R16G16B16A16_FLOAT;
	desc.Width = gViewportInfo.width;
	desc.Height = gViewportInfo.height;
	desc.MipLevels = 1u;
	desc.ArraySize = 1u;
	desc.SampleDesc.Count = 1;
	desc.SampleDesc.Quality = 0;

    pDevice->CreateTexture2D(&desc, nullptr, _resource[0].ReleaseAndGetAddressOf());
    pDevice->CreateUnorderedAccessView(_resource[0].Get(), nullptr, _uav[0].ReleaseAndGetAddressOf());
    pDevice->CreateShaderResourceView(_resource[0].Get(), nullptr, _srv[2].ReleaseAndGetAddressOf());

	//desc.Format = DXGI_FORMAT_R16G16B16A16_FLOAT;
    pDevice->CreateTexture2D(&desc, nullptr, _resource[1].ReleaseAndGetAddressOf());
    pDevice->CreateUnorderedAccessView(_resource[1].Get(), nullptr, _uav[1].ReleaseAndGetAddressOf());
    pDevice->CreateShaderResourceView(_resource[1].Get(), nullptr, _srv[3].ReleaseAndGetAddressOf());

    _cbTimestepID = NodeManager::IssueIncomingAttrID();
    _incoming[_cbTimestepID] = -1;
    _attrNames[_cbTimestepID] = { "Timestep(CB)" };

    _velocityInID = NodeManager::IssueIncomingAttrID();
    _incoming[_velocityInID] = -1;
    _attrNames[_velocityInID] = { "Velocity in" };

    _quantityInID = NodeManager::IssueIncomingAttrID();
    _incoming[_quantityInID] = -1;
    _attrNames[_quantityInID] = { "Quantity in" };

    _velocityOutID = NodeManager::IssueOutgoingAttrID();
    _outgoing[_velocityOutID] = -1;
    _attrNames[_velocityOutID] = { "Velocity out" };

    _quantityOutID = NodeManager::IssueOutgoingAttrID();
    _outgoing[_quantityOutID] = -1;
    _attrNames[_quantityOutID] = { "Quantity out" };

    std::string _path{ "./CSO/Vector4DUnorm_CS.cso" };
    std::wstring p(_path.length(), L' ');
    std::ranges::copy(_path, p.begin());

    Microsoft::WRL::ComPtr<ID3DBlob> pBlob;
    D3DReadFileToBlob(
        p.c_str(),
        pBlob.ReleaseAndGetAddressOf()
    );

    pDevice->CreateComputeShader(
        pBlob->GetBufferPointer(), 
        pBlob->GetBufferSize(),
        nullptr,
        _normalizer.ReleaseAndGetAddressOf()
    );

    pDevice->CreateTexture2D(&desc, nullptr, _velocityUnorm.ReleaseAndGetAddressOf());
    pDevice->CreateUnorderedAccessView(_velocityUnorm.Get(), nullptr, _velocityUnormView.ReleaseAndGetAddressOf());
    pDevice->CreateShaderResourceView(_velocityUnorm.Get(), nullptr, _velocityUnormSR.ReleaseAndGetAddressOf());
}

void Advection2DStage::Run(ID3D11DeviceContext& context)
{
    context.OMSetRenderTargets(0u, nullptr, nullptr);
    context.CSSetShaderResources(0u, 2u, _srv[0].GetAddressOf());
    context.CSSetUnorderedAccessViews(0u, 2u, _uav[0].GetAddressOf(), nullptr);
    context.CSSetShader(_cs.Get(), nullptr, 0u);
    context.Dispatch(
        static_cast<UINT>(ceil(static_cast<float>(gViewportInfo.width) / _groupX)),
        static_cast<UINT>(ceil(static_cast<float>(gViewportInfo.height) / _groupY)),
        _groupZ
    );
    SetBarrier(context);

    if (ImNodes::IsNodeSelected(_id)) {
        ImGui::Begin("Node Editor");

        context.CSSetShaderResources(0u, 1u, _srv[2].GetAddressOf());
        context.CSSetUnorderedAccessViews(0u, 1u, _velocityUnormView.GetAddressOf(), nullptr);
        context.CSSetShader(_normalizer.Get(), nullptr, 0u);
        context.Dispatch(
            static_cast<UINT>(ceil(static_cast<float>(gViewportInfo.width) / _groupX)),
            static_cast<UINT>(ceil(static_cast<float>(gViewportInfo.height) / _groupY)), 
            _groupZ
        );
        SetBarrier(context);

        ImGui::Text("Velocity");
        ImGui::Image(
            _velocityUnormSR.Get(), 
            ImVec2{ 
                static_cast<float>(gViewportInfo.width >> 1),
                static_cast<float>(gViewportInfo.height >> 1)
            }
        );

        ImGui::End();
    }
}

void Advection2DStage::Consume(ID3D11Resource* resource, int32_t attribute_id)
{
    if (attribute_id == _velocityInID)
        pDevice->CreateShaderResourceView(resource, nullptr, _srv[0].ReleaseAndGetAddressOf());
    else if (attribute_id == _quantityInID)
        pDevice->CreateShaderResourceView(resource, nullptr, _srv[1].ReleaseAndGetAddressOf());
}

ID3D11Resource* Advection2DStage::Expose(int32_t attribute_id)
{
    if (attribute_id == _velocityOutID)
        return static_cast<ID3D11Resource*>(_resource[0].Get());
    else if (attribute_id == _quantityOutID)
        return static_cast<ID3D11Resource*>(_resource[1].Get());
}