#include "pch.h"
#include "Sourcing2DStage.h"

#include "imnodes.h"

#include "NodeManager.h"

Sourcing2DStage::Sourcing2DStage()
    : ComputeStage{"2D-Fluid Sourcing", "./CSO/Sourcing2D_CS.cso", 32, 32, 1}
{
    _uav.resize(4);
    _srv.resize(1);
    _nullUav.resize(4);
    _nullSrv.resize(1);

    _fluidColorID = NodeManager::IssueIncomingAttrID();
    _incoming[_fluidColorID] = -1;
    _attrNames[_fluidColorID] = { "Fluid Color(CB)" };

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

Sourcing2DStage::~Sourcing2DStage()
{
}

void Sourcing2DStage::Consume(ID3D11Resource* resource, int32_t attribute_id)
{
    if (attribute_id == _velocityInID) {
		pDevice->CreateUnorderedAccessView(resource, nullptr, _uav[0].ReleaseAndGetAddressOf());
    }
    else if (attribute_id == _quantityInID) {
		pDevice->CreateUnorderedAccessView(resource, nullptr, _uav[1].ReleaseAndGetAddressOf());
    }
    else if (attribute_id == _velocityOutID) {
		pDevice->CreateUnorderedAccessView(resource, nullptr, _uav[2].ReleaseAndGetAddressOf());
    }
    else if (attribute_id == _quantityOutID) {
		pDevice->CreateUnorderedAccessView(resource, nullptr, _uav[3].ReleaseAndGetAddressOf());
    }
}

ID3D11Resource* Sourcing2DStage::Expose(int32_t attribute_id)
{
    return nullptr;
}