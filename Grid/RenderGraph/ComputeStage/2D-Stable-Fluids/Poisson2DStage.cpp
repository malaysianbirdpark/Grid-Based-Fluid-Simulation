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
    ComputeStage::Run(context);

    Microsoft::WRL::ComPtr<ID3D11Resource> src {nullptr};
    Microsoft::WRL::ComPtr<ID3D11Resource> dest {nullptr};

    if ((_uav[0] != nullptr) & (_srv[0].Get() != nullptr)) {
		_uav[0]->GetResource(src.ReleaseAndGetAddressOf());
		_srv[0]->GetResource(dest.ReleaseAndGetAddressOf());
		context.CopyResource(dest.Get(), src.Get());
    }
}

void Poisson2DStage::Consume(ID3D11Resource* resource, int32_t attribute_id)
{
    if (attribute_id == _xInID)
        pDevice->CreateShaderResourceView(resource, nullptr, _srv[0].ReleaseAndGetAddressOf());
    if (attribute_id == _bID)
        pDevice->CreateShaderResourceView(resource, nullptr, _srv[1].ReleaseAndGetAddressOf());
    else if (attribute_id == _xOutID)
        pDevice->CreateUnorderedAccessView(resource, nullptr, _uav[0].ReleaseAndGetAddressOf());
}

ID3D11Resource* Poisson2DStage::Expose(int32_t attribute_id)
{
	return nullptr;
}
