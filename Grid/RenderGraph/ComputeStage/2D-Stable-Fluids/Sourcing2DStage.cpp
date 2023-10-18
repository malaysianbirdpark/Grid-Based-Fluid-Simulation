#include "pch.h"
#include "Sourcing2DStage.h"

#include "imnodes.h"

#include "NodeManager.h"

Sourcing2DStage::Sourcing2DStage()
    : ComputeStage{"2D-Fluid Sourcing Stage", "./CSO/Sourcing2D_CS.cso", 32, 32, 1}
{
    _uav.resize(2);
    _srv.resize(1);
    _nullUav.resize(2);
    _nullSrv.resize(1);

    _fluidColorID = NodeManager::IssueIncomingAttrID();
    _incoming[_fluidColorID] = -1;
    _attrNames[_fluidColorID] = { "Fluid Color(CB)" };

    _velocityID = NodeManager::IssueOutgoingAttrID();
    _outgoing[_velocityID] = -1;
    _attrNames[_velocityID] = { "Velocity" };

    _quantityID = NodeManager::IssueOutgoingAttrID();
    _outgoing[_quantityID] = -1;
    _attrNames[_quantityID] = { "Quantity" };
}

Sourcing2DStage::~Sourcing2DStage()
{
}

void Sourcing2DStage::Consume(ID3D11Resource* resource, int32_t attribute_id)
{
    if (attribute_id == _velocityID)
		pDevice->CreateUnorderedAccessView(resource, nullptr, _uav[0].ReleaseAndGetAddressOf());
    else if (attribute_id == _quantityID)
		pDevice->CreateUnorderedAccessView(resource, nullptr, _uav[1].ReleaseAndGetAddressOf());
}

ID3D11Resource* Sourcing2DStage::Expose(int32_t attribute_id)
{
    return nullptr;
}