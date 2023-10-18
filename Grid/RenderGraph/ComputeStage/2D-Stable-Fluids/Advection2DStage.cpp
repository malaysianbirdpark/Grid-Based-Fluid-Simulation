#include "pch.h"
#include "Advection2DStage.h"

#include "imnodes.h"

#include "NodeManager.h"

Advection2DStage::Advection2DStage()
    : ComputeStage{ "2D-Fluid Advection", "./CSO/Advection2D_CS.cso", 32, 32, 1 }
{
    _uav.resize(2);
    _srv.resize(2);
    _nullUav.resize(2);
    _nullSrv.resize(2);

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
}

Advection2DStage::~Advection2DStage()
{
}

void Advection2DStage::Run(ID3D11DeviceContext& context) {
    ComputeStage::Run(context);

    Microsoft::WRL::ComPtr<ID3D11Resource> src {nullptr};
    Microsoft::WRL::ComPtr<ID3D11Resource> dest {nullptr};

    if ((_uav[0] != nullptr) & (_srv[0].Get() != nullptr)) {
		_uav[0]->GetResource(src.ReleaseAndGetAddressOf());
		_srv[0]->GetResource(dest.ReleaseAndGetAddressOf());
		context.CopyResource(dest.Get(), src.Get());
    }

    if ((_uav[1] != nullptr) & (_srv[1].Get() != nullptr)) {
		_uav[1]->GetResource(src.ReleaseAndGetAddressOf());
		_srv[1]->GetResource(dest.ReleaseAndGetAddressOf());
		context.CopyResource(dest.Get(), src.Get());
    }
}

void Advection2DStage::Consume(ID3D11Resource* resource, int32_t attribute_id)
{
    if (attribute_id == _velocityInID)
        pDevice->CreateShaderResourceView(resource, nullptr, _srv[0].ReleaseAndGetAddressOf());
    else if (attribute_id == _quantityInID)
        pDevice->CreateShaderResourceView(resource, nullptr, _srv[1].ReleaseAndGetAddressOf());
    else if (attribute_id == _velocityOutID)
        pDevice->CreateUnorderedAccessView(resource, nullptr, _uav[0].ReleaseAndGetAddressOf());
    else if (attribute_id == _quantityOutID)
        pDevice->CreateUnorderedAccessView(resource, nullptr, _uav[1].ReleaseAndGetAddressOf());
}

ID3D11Resource* Advection2DStage::Expose(int32_t attribute_id)
{
	return nullptr;
}