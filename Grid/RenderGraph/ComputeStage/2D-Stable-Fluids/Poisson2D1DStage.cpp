#include "pch.h"
#include "Poisson2D1DStage.h"

#include "NodeManager.h"

Poisson2D1DStage::Poisson2D1DStage()
    : ComputeStage{"2D1D-Poisson Solver", "./CSO/Poisson2D1D_CS.cso", 32, 32, 1}
{
    _uav.resize(2);
    _srv.resize(3);
    _nullUav.resize(2);
    _nullSrv.resize(3);

    _resource.resize(1);

	auto desc {CD3D11_TEXTURE2D_DESC{}};
	desc.BindFlags = D3D11_BIND_UNORDERED_ACCESS | D3D11_BIND_SHADER_RESOURCE;
	desc.Usage = D3D11_USAGE_DEFAULT;
	desc.CPUAccessFlags = 0u;
	desc.Format = DXGI_FORMAT_R16_FLOAT;
	desc.Width = gViewportInfo.width;
	desc.Height = gViewportInfo.height;
	desc.MipLevels = 1u;
	desc.ArraySize = 1u;
	desc.SampleDesc.Count = 1;
	desc.SampleDesc.Quality = 0;

    pDevice->CreateTexture2D(&desc, nullptr, _resource[0].ReleaseAndGetAddressOf());
    pDevice->CreateUnorderedAccessView(_resource[0].Get(), nullptr, _uav[1].ReleaseAndGetAddressOf());
    pDevice->CreateShaderResourceView(_resource[0].Get(), nullptr, _srv[1].ReleaseAndGetAddressOf());

    _xInID = NodeManager::IssueIncomingAttrID();
    _incoming[_xInID] = -1;
    _attrNames[_xInID] = { "x" };

    _bID = NodeManager::IssueIncomingAttrID();
    _incoming[_bID] = -1;
    _attrNames[_bID] = { "b" };

    _constID = NodeManager::IssueIncomingAttrID();
    _incoming[_constID] = -1;
    _attrNames[_constID] = { "Constants(CB)" };

    _xOutID = NodeManager::IssueOutgoingAttrID();
    _outgoing[_xOutID] = -1;
    _attrNames[_xOutID] = { "new x" };
}

void Poisson2D1DStage::Run(ID3D11DeviceContext& context)
{
    for (auto i {0}; i != 30; ++i) {
        context.OMSetRenderTargets(0u, nullptr, nullptr);
        context.CSSetShaderResources(0u, 1u, _srv[i & 0b1].GetAddressOf());
        context.CSSetShaderResources(1u, 1u, _srv[2].GetAddressOf());
        context.CSSetUnorderedAccessViews(0u, 1u, _uav[!(i & 0b1)].GetAddressOf(), nullptr);
        context.CSSetShader(_cs.Get(), nullptr, 0u);
        context.Dispatch(
            static_cast<UINT>(ceil(static_cast<float>(gViewportInfo.width) / _groupX)),
            static_cast<UINT>(ceil(static_cast<float>(gViewportInfo.height) / _groupY)), 
            _groupZ
        );
        SetBarrier(context);
    }
}

void Poisson2D1DStage::Consume(ID3D11Resource* resource, int32_t attribute_id)
{
    if (attribute_id == _xInID) {
		pDevice->CreateUnorderedAccessView(resource, nullptr, _uav[0].ReleaseAndGetAddressOf());
		pDevice->CreateShaderResourceView(resource, nullptr, _srv[0].ReleaseAndGetAddressOf());
    }
    else if (attribute_id == _bID) {
        pDevice->CreateShaderResourceView(resource, nullptr, _srv[2].ReleaseAndGetAddressOf());
    }
}

ID3D11Resource* Poisson2D1DStage::Expose(int32_t attribute_id)
{
    if (attribute_id == _xOutID)
		return static_cast<ID3D11Resource*>(_resource[0].Get());
}
