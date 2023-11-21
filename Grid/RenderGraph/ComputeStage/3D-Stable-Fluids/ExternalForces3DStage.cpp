#include "pch.h"
#include "ExternalForces3DStage.h"

#include <d3dcompiler.h>

#include "NodeManager.h"

ExternalForces3DStage::ExternalForces3DStage()
	: Compute3DStage{"3D-External Forces", "", 8, 8, 8}
{
    _uav.resize(5);
    _srv.resize(6);
    _nullUav.resize(5);
    _nullSrv.resize(5);

    _resource.resize(4);

	auto desc {CD3D11_TEXTURE3D_DESC{}};
	desc.BindFlags = D3D11_BIND_UNORDERED_ACCESS | D3D11_BIND_SHADER_RESOURCE;
	desc.Usage = D3D11_USAGE_DEFAULT;
	desc.CPUAccessFlags = 0u;
	desc.Width = gSimulationInfo.width;
	desc.Height = gSimulationInfo.height;
	desc.Depth = gSimulationInfo.depth;
	desc.MipLevels = 1u;

    // output velocity;
	desc.Format = DXGI_FORMAT_R16G16B16A16_FLOAT;
    pDevice->CreateTexture3D(&desc, nullptr, _resource[0].ReleaseAndGetAddressOf());
    pDevice->CreateUnorderedAccessView(_resource[0].Get(), nullptr, _uav[0].ReleaseAndGetAddressOf());
    pDevice->CreateShaderResourceView(_resource[0].Get(), nullptr, _srv[0].ReleaseAndGetAddressOf());

    // output quantity;
	desc.Format = DXGI_FORMAT_R16G16B16A16_FLOAT;
    pDevice->CreateTexture3D(&desc, nullptr, _resource[1].ReleaseAndGetAddressOf());
    pDevice->CreateUnorderedAccessView(_resource[1].Get(), nullptr, _uav[1].ReleaseAndGetAddressOf());
    pDevice->CreateShaderResourceView(_resource[1].Get(), nullptr, _srv[1].ReleaseAndGetAddressOf());

    // intermediate velocity;
	desc.Format = DXGI_FORMAT_R16G16B16A16_FLOAT;
    pDevice->CreateTexture3D(&desc, nullptr, _resource[2].ReleaseAndGetAddressOf());
    pDevice->CreateUnorderedAccessView(_resource[2].Get(), nullptr, _uav[2].ReleaseAndGetAddressOf());
    pDevice->CreateShaderResourceView(_resource[2].Get(), nullptr, _srv[2].ReleaseAndGetAddressOf());

    // vorticity;
	desc.Format = DXGI_FORMAT_R16G16B16A16_FLOAT;
    pDevice->CreateTexture3D(&desc, nullptr, _resource[3].ReleaseAndGetAddressOf());
    pDevice->CreateUnorderedAccessView(_resource[3].Get(), nullptr, _uav[3].ReleaseAndGetAddressOf());
    pDevice->CreateShaderResourceView(_resource[3].Get(), nullptr, _srv[3].ReleaseAndGetAddressOf());

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
        std::string _path{"./CSO/Gravity3D_CS.cso"};
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
            _gravityCS.ReleaseAndGetAddressOf()
        );
    }

    {
        std::string _path{"./CSO/Buoyancy3D_CS.cso"};
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
            _buoyancyCS.ReleaseAndGetAddressOf()
        );
    }
    
    {
        std::string _path{"./CSO/Combustion3D_CS.cso"};
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
            _combustionCS.ReleaseAndGetAddressOf()
        );
    }

    {
        std::string _path{"./CSO/Vorticity3D_CS.cso"};
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
            _vorticityCS.ReleaseAndGetAddressOf()
        );
    }

    {
        std::string _path{"./CSO/ConfineVorticity3D_CS.cso"};
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
            _confineVorticityCS.ReleaseAndGetAddressOf()
        );
    }
}

void ExternalForces3DStage::Run(ID3D11DeviceContext& context)
{
    context.OMSetRenderTargets(0u, nullptr, nullptr);

    //context.CSSetShaderResources(0u, 1u, _srv[0].GetAddressOf());
    //context.CSSetUnorderedAccessViews(0u, 1u, _uav[1].GetAddressOf(), nullptr);
    //context.CSSetShader(_gravityCS.Get(), nullptr, 0u);
    //context.Dispatch(
    //    static_cast<UINT>(ceil(static_cast<float>(gSimulationInfo.width) / _groupX)),
    //    static_cast<UINT>(ceil(static_cast<float>(gSimulationInfo.height) / _groupY)),
    //    static_cast<UINT>(ceil(static_cast<float>(gSimulationInfo.depth) / _groupZ))
    //);
    //SetBarrier(context);

    // combustion
    context.CSSetShaderResources(0u, 2u, _srv[4].GetAddressOf());
    context.CSSetUnorderedAccessViews(0u, 2u, _uav[1].GetAddressOf(), nullptr);
    context.CSSetShader(_combustionCS.Get(), nullptr, 0u);
    context.Dispatch(
        static_cast<UINT>(ceil(static_cast<float>(gSimulationInfo.width) / _groupX)),
        static_cast<UINT>(ceil(static_cast<float>(gSimulationInfo.height) / _groupY)),
        static_cast<UINT>(ceil(static_cast<float>(gSimulationInfo.depth) / _groupZ))
    );
    SetBarrier(context);

    // calculate and apply buoyancy
    context.CSSetShaderResources(0u, 2u, _srv[1].GetAddressOf());
    context.CSSetUnorderedAccessViews(0u, 1u, _uav[0].GetAddressOf(), nullptr);
    context.CSSetShader(_buoyancyCS.Get(), nullptr, 0u);
    context.Dispatch(
        static_cast<UINT>(ceil(static_cast<float>(gSimulationInfo.width) / _groupX)),
        static_cast<UINT>(ceil(static_cast<float>(gSimulationInfo.height) / _groupY)),
        static_cast<UINT>(ceil(static_cast<float>(gSimulationInfo.depth) / _groupZ))
    );
    SetBarrier(context);

    // calculate vorticity
    context.CSSetShaderResources(0u, 1u, _srv[0].GetAddressOf());
    context.CSSetUnorderedAccessViews(0u, 1u, _uav[3].GetAddressOf(), nullptr);
    context.CSSetShader(_vorticityCS.Get(), nullptr, 0u);
    context.Dispatch(
        static_cast<UINT>(ceil(static_cast<float>(gSimulationInfo.width) / _groupX)),
        static_cast<UINT>(ceil(static_cast<float>(gSimulationInfo.height) / _groupY)),
        static_cast<UINT>(ceil(static_cast<float>(gSimulationInfo.depth) / _groupZ))
    );
    SetBarrier(context);

    // apply vorticity
    context.CSSetShaderResources(0u, 1u, _srv[3].GetAddressOf());
    context.CSSetUnorderedAccessViews(0u, 1u, _uav[0].GetAddressOf(), nullptr);
    context.CSSetShader(_confineVorticityCS.Get(), nullptr, 0u);
    context.Dispatch(
        static_cast<UINT>(ceil(static_cast<float>(gSimulationInfo.width) / _groupX)),
        static_cast<UINT>(ceil(static_cast<float>(gSimulationInfo.height) / _groupY)),
        static_cast<UINT>(ceil(static_cast<float>(gSimulationInfo.depth) / _groupZ))
    );
    SetBarrier(context);

  //  if (_uav[0].Get()) {
		//ID3D11Resource* src{ nullptr };
		//_uav[0]->GetResource(&src);
		//context.CopyResource(_resource[0].Get(), src);
  //  }
}

void ExternalForces3DStage::Consume(ID3D11Resource* resource, int32_t attribute_id)
{
    if (attribute_id == _velocityInID) {
        pDevice->CreateShaderResourceView(resource, nullptr, _srv[4].ReleaseAndGetAddressOf());
    }
    else if (attribute_id == _quantityInID) {
        pDevice->CreateShaderResourceView(resource, nullptr, _srv[5].ReleaseAndGetAddressOf());
    }
}

ID3D11Resource* ExternalForces3DStage::Expose(int32_t attribute_id)
{
    if (attribute_id == _velocityOutID)
        return static_cast<ID3D11Resource*>(_resource[0].Get());
    else if (attribute_id == _quantityOutID)
        return static_cast<ID3D11Resource*>(_resource[1].Get());
	return nullptr;
}
