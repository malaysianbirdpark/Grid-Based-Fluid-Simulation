#include "pch.h"
#include "BackBufferStage.h"

#include "imnodes.h"

#include "NodeManager.h"

BackBufferStage::BackBufferStage(IDXGISwapChain& swap_chain)
    : ResourceStage{"Back-Buffer"}
{
    swap_chain.GetBuffer(0u, IID_PPV_ARGS(_resource.ReleaseAndGetAddressOf()));
    _incoming[NodeManager::IssueIncomingAttrID()] = -1;
    _attrNames[NodeManager::LastIncomingAttrID()] = { "Src" };
    _outgoing[NodeManager::IssueOutgoingAttrID()] = -1;
    _attrNames[NodeManager::LastOutgoingAttrID()] = { "Dest" };
}

BackBufferStage::~BackBufferStage() {
}

ID3D11Resource* BackBufferStage::Expose(int32_t attribute_id) const {
    return _resource.Get();
}
