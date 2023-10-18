#include "pch.h"
#include "ViewportStage.h"

#include "NodeManager.h"

ViewportStage::ViewportStage()
	: ResourceStage{"Viewport"}
{
    _incoming[NodeManager::IssueIncomingAttrID()] = -1;
    _attrNames[NodeManager::LastIncomingAttrID()] = { "Src" };
    _outgoing[NodeManager::IssueOutgoingAttrID()] = -1;
    _attrNames[NodeManager::LastOutgoingAttrID()] = { "Dest" };
}

ID3D11Resource* ViewportStage::Expose(int32_t attribute_id) const
{
    ID3D11Resource* resource{};
    pViewportSRV->GetResource(&resource);
    return resource;
}
