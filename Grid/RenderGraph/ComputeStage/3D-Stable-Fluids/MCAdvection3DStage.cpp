#include "pch.h"
#include "MCAdvection3DStage.h"

#include <d3dcompiler.h>

#include "imnodes.h"
#include "NodeManager.h"

MCAdvection3DStage::MCAdvection3DStage()
    : Compute3DStage{ "3D-MCAdvection", "", 8, 8, 8 }
{
    _uav.resize(8);
    _srv.resize(6);
    _nullUav.resize(8);
    _nullSrv.resize(6);

    _resource.resize(6);

	auto desc {CD3D11_TEXTURE3D_DESC{}};
	desc.BindFlags = D3D11_BIND_UNORDERED_ACCESS | D3D11_BIND_SHADER_RESOURCE;
	desc.Usage = D3D11_USAGE_DEFAULT;
	desc.CPUAccessFlags = 0u;
	desc.Width = gSimulationInfo.width;
	desc.Height = gSimulationInfo.height;
	desc.Depth = gSimulationInfo.depth;
	desc.MipLevels = 1u;

    // n_1_hat
	desc.Format = DXGI_FORMAT_R16G16B16A16_FLOAT;
    pDevice->CreateTexture3D(&desc, nullptr, _resource[0].ReleaseAndGetAddressOf());
    pDevice->CreateUnorderedAccessView(_resource[0].Get(), nullptr, _uav[2].ReleaseAndGetAddressOf());
    pDevice->CreateShaderResourceView(_resource[0].Get(), nullptr, _srv[2].ReleaseAndGetAddressOf());

	desc.Format = DXGI_FORMAT_R16G16B16A16_FLOAT;
    pDevice->CreateTexture3D(&desc, nullptr, _resource[1].ReleaseAndGetAddressOf());
    pDevice->CreateUnorderedAccessView(_resource[1].Get(), nullptr, _uav[3].ReleaseAndGetAddressOf());
    pDevice->CreateShaderResourceView(_resource[1].Get(), nullptr, _srv[3].ReleaseAndGetAddressOf());

    // n_hat
	desc.Format = DXGI_FORMAT_R16G16B16A16_FLOAT;
    pDevice->CreateTexture3D(&desc, nullptr, _resource[2].ReleaseAndGetAddressOf());
    pDevice->CreateUnorderedAccessView(_resource[2].Get(), nullptr, _uav[4].ReleaseAndGetAddressOf());
    pDevice->CreateShaderResourceView(_resource[2].Get(), nullptr, _srv[4].ReleaseAndGetAddressOf());

	desc.Format = DXGI_FORMAT_R16G16B16A16_FLOAT;
    pDevice->CreateTexture3D(&desc, nullptr, _resource[3].ReleaseAndGetAddressOf());
    pDevice->CreateUnorderedAccessView(_resource[3].Get(), nullptr, _uav[5].ReleaseAndGetAddressOf());
    pDevice->CreateShaderResourceView(_resource[3].Get(), nullptr, _srv[5].ReleaseAndGetAddressOf());

    // final
	desc.Format = DXGI_FORMAT_R16G16B16A16_FLOAT;
    pDevice->CreateTexture3D(&desc, nullptr, _resource[4].ReleaseAndGetAddressOf());
    pDevice->CreateUnorderedAccessView(_resource[4].Get(), nullptr, _uav[6].ReleaseAndGetAddressOf());

	desc.Format = DXGI_FORMAT_R16G16B16A16_FLOAT;
    pDevice->CreateTexture3D(&desc, nullptr, _resource[5].ReleaseAndGetAddressOf());
    pDevice->CreateUnorderedAccessView(_resource[5].Get(), nullptr, _uav[7].ReleaseAndGetAddressOf());

    _velocityInID = NodeManager::IssueIncomingAttrID();
    _incoming[_velocityInID] = -1;
    _attrNames[_velocityInID] = { "Velocity in" };

    _velocityOutID = NodeManager::IssueOutgoingAttrID();
    _outgoing[_velocityOutID] = -1;
    _attrNames[_velocityOutID] = { "Velocity out" };

    _quantityInID = NodeManager::IssueIncomingAttrID();
    _incoming[_quantityInID] = -1;
    _attrNames[_quantityInID] = { "Quantity in" };

    _quantityOutID = NodeManager::IssueOutgoingAttrID();
    _outgoing[_quantityOutID] = -1;
    _attrNames[_quantityOutID] = { "Quantity out" };

    {
        std::string _path{"./CSO/FirstAdvection3D_CS.cso"};
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
            _firstAdvectionCS.ReleaseAndGetAddressOf()
        );
    }

    {
        std::string _path{"./CSO/ReverseAdvection3D_CS.cso"};
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
            _reverseAdvectionCS.ReleaseAndGetAddressOf()
        );
    }

    {
        std::string _path{"./CSO/MacCormackAdvection3D_CS.cso"};
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
            _MCAdvectionCS.ReleaseAndGetAddressOf()
        );
    }
}

void MCAdvection3DStage::Run(ID3D11DeviceContext& context)
{

    context.OMSetRenderTargets(0u, nullptr, nullptr);

    context.CSSetShaderResources(0u, 2u, _srv[0].GetAddressOf());
    context.CSSetUnorderedAccessViews(0u, 2u, _uav[2].GetAddressOf(), nullptr);
    context.CSSetShader(_firstAdvectionCS.Get(), nullptr, 0u);
    context.Dispatch(
        static_cast<UINT>(ceil(static_cast<float>(gSimulationInfo.width) / _groupX)),
        static_cast<UINT>(ceil(static_cast<float>(gSimulationInfo.height) / _groupY)),
        static_cast<UINT>(ceil(static_cast<float>(gSimulationInfo.depth) / _groupZ))
    );
    SetBarrier(context);

    context.CSSetShaderResources(0u, 2u, _srv[2].GetAddressOf());
    context.CSSetUnorderedAccessViews(0u, 2u, _uav[4].GetAddressOf(), nullptr);
    context.CSSetShader(_reverseAdvectionCS.Get(), nullptr, 0u);
    context.Dispatch(
        static_cast<UINT>(ceil(static_cast<float>(gSimulationInfo.width) / _groupX)),
        static_cast<UINT>(ceil(static_cast<float>(gSimulationInfo.height) / _groupY)),
        static_cast<UINT>(ceil(static_cast<float>(gSimulationInfo.depth) / _groupZ))
    );
    SetBarrier(context);

    context.CSSetShaderResources(0u, 6u, _srv[0].GetAddressOf());
    context.CSSetUnorderedAccessViews(0u, 2u, _uav[6].GetAddressOf(), nullptr);
    context.CSSetShader(_MCAdvectionCS.Get(), nullptr, 0u);
    context.Dispatch(
        static_cast<UINT>(ceil(static_cast<float>(gSimulationInfo.width) / _groupX)),
        static_cast<UINT>(ceil(static_cast<float>(gSimulationInfo.height) / _groupY)),
        static_cast<UINT>(ceil(static_cast<float>(gSimulationInfo.depth) / _groupZ))
    );
    SetBarrier(context);
}

void MCAdvection3DStage::Consume(ID3D11Resource* resource, int32_t attribute_id)
{
    if (attribute_id == _velocityInID)
        pDevice->CreateShaderResourceView(resource, nullptr, _srv[0].ReleaseAndGetAddressOf());
    else if (attribute_id == _quantityInID)
        pDevice->CreateShaderResourceView(resource, nullptr, _srv[1].ReleaseAndGetAddressOf());
}

ID3D11Resource* MCAdvection3DStage::Expose(int32_t attribute_id)
{
    if (attribute_id == _velocityOutID)
        return static_cast<ID3D11Resource*>(_resource[4].Get());
    else if (attribute_id == _quantityOutID)
        return static_cast<ID3D11Resource*>(_resource[5].Get());
    return nullptr;
}
