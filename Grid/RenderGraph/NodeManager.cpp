#include "pch.h"
#include "NodeManager.h"

#include <iostream>

int32_t NodeManager::IssueNodeID() {
    return _nodeID++;
}

int32_t NodeManager::IssueIncomingAttrID()
{
    _attrOwner[_incomingAttrID] = _nodeID;
    return _incomingAttrID++;
}

int32_t NodeManager::IssueOutgoingAttrID()
{
    _attrOwner[_outgoingAttrID] = _nodeID;
    return _outgoingAttrID++;
}

int32_t NodeManager::LastNodeID() {
    return _nodeID - 1;
}

int32_t NodeManager::LastIncomingAttrID()
{
    return _incomingAttrID - 1;
}

int32_t NodeManager::LastOutgoingAttrID()
{
    return _outgoingAttrID - 1;
}

int32_t NodeManager::GetOwner(int32_t attr_id)
{
    if (_attrOwner.contains(attr_id))
        return _attrOwner[attr_id];
    else
        return -1;
}

