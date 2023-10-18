#include "pch.h"
#include "Divergence2DStage.h"

#include "NodeManager.h"

Divergence2DStage::Divergence2DStage()
	: ComputeStage{"2D-Divergence Operation", "./CSO/Divergence2D_CS.cso", 32, 32, 1}
{
    _uav.resize(1);
    _srv.resize(1);
    _nullUav.resize(1);
    _nullSrv.resize(1);

    _xInID = NodeManager::IssueIncomingAttrID();
    _incoming[_xInID] = -1;
    _attrNames[_xInID] = { "x in" };

    _xOutID = NodeManager::IssueOutgoingAttrID();
    _outgoing[_xOutID] = -1;
    _attrNames[_xOutID] = { "x out" };
}

void Divergence2DStage::Run(ID3D11DeviceContext& context)
{
    ComputeStage::Run(context);
}

void Divergence2DStage::Consume(ID3D11Resource* resource, int32_t attribute_id)
{
    if (attribute_id == _xInID)
        pDevice->CreateShaderResourceView(resource, nullptr, _srv[0].ReleaseAndGetAddressOf());
    else if (attribute_id == _xOutID)
        pDevice->CreateUnorderedAccessView(resource, nullptr, _uav[0].ReleaseAndGetAddressOf());
}

ID3D11Resource* Divergence2DStage::Expose(int32_t attribute_id)
{
	return nullptr;
}
