#include "pch.h"
#include "Initializer3DStage.h"

#include "NodeManager.h"

Initializer3DStage::Initializer3DStage()
    : Compute3DStage{"Initializer", "./CSO/Initializer3D_CS.cso", 8, 8, 8}
{
    _uav.resize(1);
    _srv.resize(1);
    _nullUav.resize(1);
    _nullSrv.resize(1);

    _targetID = NodeManager::IssueOutgoingAttrID();
    _outgoing[_targetID] = -1;
    _attrNames[_targetID] = { "Target" };
}

void Initializer3DStage::Consume(ID3D11Resource* resource, int32_t attribute_id) {
    if (attribute_id == _targetID)
        pDevice->CreateUnorderedAccessView(resource, nullptr, _uav[0].ReleaseAndGetAddressOf());
}

ID3D11Resource* Initializer3DStage::Expose(int32_t attribute_id) {
    return nullptr;
}

