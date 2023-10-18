#include "pch.h"
#include "GradientSubtract2DStage.h"

#include "NodeManager.h"

GradientSubtract2DStage::GradientSubtract2DStage()
	: ComputeStage{"2D-GradientSubtract", "./CSO/GradientSub2D_CS.cso", 32, 32, 1}
{
    _uav.resize(1);
    _srv.resize(2);
    _nullUav.resize(1);
    _nullSrv.resize(2);

    _xInID = NodeManager::IssueIncomingAttrID();
    _incoming[_xInID] = -1;
    _attrNames[_xInID] = { "x in" };

    _subfromID = NodeManager::IssueIncomingAttrID();
    _incoming[_subfromID] = -1;
    _attrNames[_subfromID] = { "Subtract from" };

    _outID = NodeManager::IssueOutgoingAttrID();
    _outgoing[_outID] = -1;
    _attrNames[_outID] = { "Result" };

}

void GradientSubtract2DStage::Run(ID3D11DeviceContext& context)
{
    ComputeStage::Run(context);
}

void GradientSubtract2DStage::Consume(ID3D11Resource* resource, int32_t attribute_id)
{
    if (attribute_id == _xInID)
        pDevice->CreateShaderResourceView(resource, nullptr, _srv[0].ReleaseAndGetAddressOf());
    else if (attribute_id == _subfromID)
        pDevice->CreateShaderResourceView(resource, nullptr, _srv[1].ReleaseAndGetAddressOf());
    else if (attribute_id == _outID)
        pDevice->CreateUnorderedAccessView(resource, nullptr, _uav[0].ReleaseAndGetAddressOf());
}

ID3D11Resource* GradientSubtract2DStage::Expose(int32_t attribute_id)
{
	return nullptr;
}
