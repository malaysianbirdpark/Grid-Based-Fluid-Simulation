#include "pch.h"
#include "Poisson2DStage.h"

#include "NodeManager.h"

Poisson2DStage::Poisson2DStage()
    : ComputeStage{"2D-Poisson Solver", "./CSO/Poisson2D_CS.cso", 32, 32, 1}
{
    _uav.resize(1);
    _srv.resize(2);
    _nullUav.resize(2);
    _nullSrv.resize(2);

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
    pDevice->CreateUnorderedAccessView(_resource[0].Get(), nullptr, _uav[0].ReleaseAndGetAddressOf());

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

Poisson2DStage::~Poisson2DStage()
{
}

void Poisson2DStage::Run(ID3D11DeviceContext& context)
{
    ID3D11Resource* src {nullptr};
    ID3D11Resource* dest {nullptr};
    for (auto i {0}; i != 40; ++i) {
        ComputeStage::Run(context);

        if ((_uav[0] != nullptr) & (_srv[0].Get() != nullptr)) {
            _uav[0]->GetResource(&src);
            _srv[0]->GetResource(&dest);

            context.CopyResource(dest, src);
        }
    }
}

void Poisson2DStage::Consume(ID3D11Resource* resource, int32_t attribute_id)
{
    if (attribute_id == _xInID)
        pDevice->CreateShaderResourceView(resource, nullptr, _srv[0].ReleaseAndGetAddressOf());
    else if (attribute_id == _bID)
        pDevice->CreateShaderResourceView(resource, nullptr, _srv[1].ReleaseAndGetAddressOf());
}

ID3D11Resource* Poisson2DStage::Expose(int32_t attribute_id)
{
    ID3D11Resource* resource;
    if (attribute_id == _xOutID)
        _uav[0]->GetResource(&resource);
    return resource;
}
