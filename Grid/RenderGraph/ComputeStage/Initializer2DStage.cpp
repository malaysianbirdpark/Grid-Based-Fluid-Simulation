#include "pch.h"
#include "Initializer2DStage.h"

#include "NodeManager.h"

Initializer2DStage::Initializer2DStage()
    : Compute2DStage{"Initializer", "./CSO/Initializer2D_CS.cso", 32, 32, 1}
{
    _uav.resize(1);
    _srv.resize(1);
    _nullUav.resize(1);
    _nullSrv.resize(1);

    _targetID = NodeManager::IssueOutgoingAttrID();
    _outgoing[_targetID] = -1;
    _attrNames[_targetID] = { "Target" };
}

void Initializer2DStage::Consume(ID3D11Resource* resource, int32_t attribute_id) {
    if (attribute_id == _targetID)
        pDevice->CreateUnorderedAccessView(resource, nullptr, _uav[0].ReleaseAndGetAddressOf());
}

ID3D11Resource* Initializer2DStage::Expose(int32_t attribute_id) {
    return nullptr;
}
