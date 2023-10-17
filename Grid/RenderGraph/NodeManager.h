#pragma once

#include <cstdint>
#include <unordered_map>

class NodeManager {
public: static int32_t IssueNodeID();
public: static int32_t IssueIncomingAttrID();
public: static int32_t IssueOutgoingAttrID();
public: static int32_t LastNodeID();
public: static int32_t LastIncomingAttrID();
public: static int32_t LastOutgoingAttrID();
public: static int32_t GetOwner(int32_t attr_id);
private:
	inline static int32_t _nodeID {};
	inline static int32_t _incomingAttrID {};
	inline static int32_t _outgoingAttrID {256};
	inline static std::unordered_map<int32_t, int32_t> _attrOwner;
};

