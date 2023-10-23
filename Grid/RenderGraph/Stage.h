#pragma once

#include <vector>
#include <string>
#include <memory>
#include <variant>
#include "unordered_map"

#include "DrawStage.h"
#include "Compute2DStage.h"
#include "ResourceStage.h"
#include "CopyStage.h"
#include "ConstantBufferStage.h"

namespace Stage {
	using Stage =
		std::variant<
			std::shared_ptr<DrawStage>,
			std::shared_ptr<Compute2DStage>,
		    std::shared_ptr<ResourceStage>,
		    std::shared_ptr<CopyStage>,
		    std::shared_ptr<ConstantBufferStage>
		>;

	struct Run {
		Run(ID3D11DeviceContext& context) : _context{ context } {}
		void operator() (std::shared_ptr<DrawStage> const& stage) const { stage->Run(_context); }
		void operator() (std::shared_ptr<Compute2DStage> const& stage) const { stage->Run(_context); }
		void operator() (std::shared_ptr<ResourceStage> const& stage) const {}
		void operator() (std::shared_ptr<CopyStage> const& stage) const { stage->Run(_context);  }
		void operator() (std::shared_ptr<ConstantBufferStage> const& stage) const { stage->Run(_context);  }
	private:
		ID3D11DeviceContext& _context;
	};

	struct Update {
		Update(ID3D11DeviceContext& context) : _context{ context } {}
		void operator() (std::shared_ptr<DrawStage> const& stage) const { stage->Update(_context); }
		void operator() (std::shared_ptr<Compute2DStage> const& stage) const {}
		void operator() (std::shared_ptr<ResourceStage> const& stage) const {}
		void operator() (std::shared_ptr<CopyStage> const& stage) const {}
		void operator() (std::shared_ptr<ConstantBufferStage> const& stage) const {}
	private:
		ID3D11DeviceContext& _context;
	};

	struct AddOutgoing {
		AddOutgoing(int32_t node_id, int32_t attr_id) : _nodeID{ node_id }, _attrID{attr_id} {}
		void operator() (std::shared_ptr<DrawStage> const& stage) const { stage->_outgoing[_attrID] = _nodeID;  }
		void operator() (std::shared_ptr<Compute2DStage> const& stage) const { stage->_outgoing[_attrID] = _nodeID;  }
		void operator() (std::shared_ptr<ResourceStage> const& stage) const { stage->_outgoing[_attrID] = _nodeID;  }
		void operator() (std::shared_ptr<CopyStage> const& stage) const { stage->_outgoing[_attrID] = _nodeID;  }
		void operator() (std::shared_ptr<ConstantBufferStage> const& stage) const { stage->_outgoing[_attrID] = _nodeID;  }
	private:
		int32_t _nodeID;
		int32_t _attrID;
	};

	struct GetOutgoings {
		std::unordered_map<int32_t, int32_t> const& operator() (std::shared_ptr<DrawStage> const& stage) const { return stage->_outgoing; }
		std::unordered_map<int32_t, int32_t> const& operator() (std::shared_ptr<Compute2DStage> const& stage) const { return stage->_outgoing; }
		std::unordered_map<int32_t, int32_t> const& operator() (std::shared_ptr<ResourceStage> const& stage) const { return stage->_outgoing; }
		std::unordered_map<int32_t, int32_t> const& operator() (std::shared_ptr<CopyStage> const& stage) const { return stage->_outgoing; }
		std::unordered_map<int32_t, int32_t> const& operator() (std::shared_ptr<ConstantBufferStage> const& stage) const { return stage->_outgoing; }
	};

	struct AddIncoming {
		AddIncoming(int32_t node_id, int32_t attr_id) : _nodeID{ node_id }, _attrID{ attr_id } {}
		void operator() (std::shared_ptr<DrawStage> const& stage) const { stage->_incoming[_attrID] = _nodeID; }
		void operator() (std::shared_ptr<Compute2DStage> const& stage) const { stage->_incoming[_attrID] = _nodeID; }
		void operator() (std::shared_ptr<ResourceStage> const& stage) const { stage->_incoming[_attrID] = _nodeID; }
		void operator() (std::shared_ptr<CopyStage> const& stage) const { stage->_incoming[_attrID] = _nodeID; }
		void operator() (std::shared_ptr<ConstantBufferStage> const& stage) const { stage->_incoming[_attrID] = _nodeID; }
	private:
		int32_t _nodeID;
		int32_t _attrID;
	};

	struct GetIncoming {
		std::unordered_map<int32_t, int32_t> operator() (std::shared_ptr<DrawStage> const& stage) const { return stage->_incoming;  }
		std::unordered_map<int32_t, int32_t> operator() (std::shared_ptr<Compute2DStage> const& stage) const { return stage->_incoming;  }
		std::unordered_map<int32_t, int32_t> operator() (std::shared_ptr<ResourceStage> const& stage) const { return stage->_incoming;  }
		std::unordered_map<int32_t, int32_t> operator() (std::shared_ptr<CopyStage> const& stage) const { return stage->_incoming;  }
		std::unordered_map<int32_t, int32_t> operator() (std::shared_ptr<ConstantBufferStage> const& stage) const { return stage->_incoming;  }
	};

	struct GetID {
		int32_t operator() (std::shared_ptr<DrawStage> const& stage) const { return stage->_id;  }
		int32_t operator() (std::shared_ptr<Compute2DStage> const& stage) const { return stage->_id;  }
		int32_t operator() (std::shared_ptr<ResourceStage> const& stage) const { return stage->_id;  }
		int32_t operator() (std::shared_ptr<CopyStage> const& stage) const { return stage->_id;  }
		int32_t operator() (std::shared_ptr<ConstantBufferStage> const& stage) const { return stage->_id;  }
	};

	struct SetID {
		SetID(int32_t id) : _id{id} {}
		void operator() (std::shared_ptr<DrawStage> const& stage) const { stage->_id = _id; }
		void operator() (std::shared_ptr<Compute2DStage> const& stage) const { stage->_id = _id; }
		void operator() (std::shared_ptr<ResourceStage> const& stage) const { stage->_id = _id; }
		void operator() (std::shared_ptr<CopyStage> const& stage) const { stage->_id = _id; }
		void operator() (std::shared_ptr<ConstantBufferStage> const& stage) const { stage->_id = _id; }
	private:
		int32_t _id;
	};

	struct GetName {
		char const* operator() (std::shared_ptr<DrawStage> const& stage) const { return stage->_name.c_str(); }
		char const* operator() (std::shared_ptr<Compute2DStage> const& stage) const { return stage->_name.c_str(); }
		char const* operator() (std::shared_ptr<ResourceStage> const& stage) const { return stage->_name.c_str(); }
		char const* operator() (std::shared_ptr<CopyStage> const& stage) const { return stage->_name.c_str(); }
		char const* operator() (std::shared_ptr<ConstantBufferStage> const& stage) const { return stage->_name.c_str(); }
	};

	struct GetStageName {
		char const* operator() (std::shared_ptr<DrawStage> const& stage) const { return stage->_stageName.c_str(); }
		char const* operator() (std::shared_ptr<Compute2DStage> const& stage) const { return stage->_stageName.c_str(); }
		char const* operator() (std::shared_ptr<ResourceStage> const& stage) const { return stage->_stageName.c_str(); }
		char const* operator() (std::shared_ptr<CopyStage> const& stage) const { return stage->_stageName.c_str(); }
		char const* operator() (std::shared_ptr<ConstantBufferStage> const& stage) const { return stage->_stageName.c_str(); }
	};

	struct RenderNode {
		void operator() (std::shared_ptr<DrawStage> const& stage) const { stage->RenderNode(); }
		void operator() (std::shared_ptr<Compute2DStage> const& stage) const { stage->RenderNode(); }
		void operator() (std::shared_ptr<ResourceStage> const& stage) const { stage->RenderNode(); }
		void operator() (std::shared_ptr<CopyStage> const& stage) const { stage->RenderNode(); }
		void operator() (std::shared_ptr<ConstantBufferStage> const& stage) const { stage->RenderNode(); }
	};

	struct Expose {
		Expose(int32_t attribute_id) : _attribute{attribute_id} {}
		ID3D11Resource* operator() (std::shared_ptr<DrawStage> const& stage) const { return stage->Expose(_attribute);  }
		ID3D11Resource* operator() (std::shared_ptr<Compute2DStage> const& stage) const { return stage->Expose(_attribute); }
		ID3D11Resource* operator() (std::shared_ptr<ResourceStage> const& stage) const { return stage->Expose(); }
		ID3D11Resource* operator() (std::shared_ptr<CopyStage> const& stage) const { return nullptr; }
		ID3D11Resource* operator() (std::shared_ptr<ConstantBufferStage> const& stage) const { return nullptr; }
	private:
		int32_t _attribute;
	};
	
	struct Consume {
		Consume(ID3D11Resource* resource, int32_t attribute_id) : _resource{resource}, _attribute{attribute_id} {}
		void operator() (std::shared_ptr<DrawStage> const& stage) const {}
		void operator() (std::shared_ptr<Compute2DStage> const& stage) const { if(_resource) stage->Consume(_resource, _attribute); }
		void operator() (std::shared_ptr<ResourceStage> const& stage) const {}
		void operator() (std::shared_ptr<CopyStage> const& stage) const { if (_resource) stage->Consume(_resource, _attribute); }
		void operator() (std::shared_ptr<ConstantBufferStage> const& stage) const {}
	private:
		ID3D11Resource* _resource;
		int32_t _attribute;
	};
}
