#pragma once

#include <vector>
#include <string>
#include <memory>
#include <variant>

#include "DrawStage.h"
#include "ComputeStage.h"
#include "ResourceStage.h"

namespace Stage {
	struct RootDummyStage {
		int32_t                    _id{0};
		std::string                _name{ "Root" };
        inline static std::string  _stageName{ "DummyStage" };
		std::vector<int32_t>       _incoming{};
		std::vector<int32_t>       _outgoing{};

		void RenderNode() const;
	};

	using Stage =
		std::variant <
			std::shared_ptr<RootDummyStage>,
			std::shared_ptr<DrawStage>,
			std::shared_ptr<ComputeStage>,
		    std::shared_ptr<ResourceStage>
		>;

	struct Run {
		Run(ID3D11DeviceContext& context) : _context{ context } {}
		void operator() (std::shared_ptr<RootDummyStage> stage) const {}
		void operator() (std::shared_ptr<DrawStage> const& stage) const { stage->Run(_context); }
		void operator() (std::shared_ptr<ComputeStage> const& stage) const { stage->Run(_context); }
		void operator() (std::shared_ptr<ResourceStage> const& stage) const {}
	private:
		ID3D11DeviceContext& _context;
	};

	struct AddOutgoing {
		AddOutgoing(int32_t child_id) : _childID{child_id} {}
		void operator() (std::shared_ptr<RootDummyStage> const& stage) { stage->_outgoing.emplace_back(_childID); }
		void operator() (std::shared_ptr<DrawStage> const& stage) { stage->_outgoing.emplace_back(_childID);  }
		void operator() (std::shared_ptr<ComputeStage> const& stage) { stage->_outgoing.emplace_back(_childID);  }
		void operator() (std::shared_ptr<ResourceStage> const& stage) { stage->_outgoing.emplace_back(_childID);  }
	private:
		int32_t _childID;
	};

	struct GetOutgoings {
		std::vector<int32_t> const& operator() (std::shared_ptr<RootDummyStage> const& stage) const { return stage->_outgoing; }
		std::vector<int32_t> const& operator() (std::shared_ptr<DrawStage> const& stage) const { return stage->_outgoing; }
		std::vector<int32_t> const& operator() (std::shared_ptr<ComputeStage> const& stage) const { return stage->_outgoing; }
		std::vector<int32_t> const& operator() (std::shared_ptr<ResourceStage> const& stage) const { return stage->_outgoing; }
	};

	struct AddIncoming {
		AddIncoming(int32_t from) : _from{from} {}
		void operator() (std::shared_ptr<RootDummyStage> const& stage) const {}
		void operator() (std::shared_ptr<DrawStage> const& stage) const { stage->_incoming.emplace_back(_from); }
		void operator() (std::shared_ptr<ComputeStage> const& stage) const { stage->_incoming.emplace_back(_from); }
		void operator() (std::shared_ptr<ResourceStage> const& stage) const { stage->_incoming.emplace_back(_from); }
	private:
		int32_t _from;
	};

	struct GetIncoming {
		std::vector<int32_t> operator() (std::shared_ptr<RootDummyStage> const& stage) const { return stage->_incoming; }
		std::vector<int32_t> operator() (std::shared_ptr<DrawStage> const& stage) const { return stage->_incoming;  }
		std::vector<int32_t> operator() (std::shared_ptr<ComputeStage> const& stage) const { return stage->_incoming;  }
		std::vector<int32_t> operator() (std::shared_ptr<ResourceStage> const& stage) const { return stage->_incoming;  }
	};

	struct GetID {
		int32_t operator() (std::shared_ptr<RootDummyStage> const& stage) const { return stage->_id; }
		int32_t operator() (std::shared_ptr<DrawStage> const& stage) const { return stage->_id;  }
		int32_t operator() (std::shared_ptr<ComputeStage> const& stage) const { return stage->_id;  }
		int32_t operator() (std::shared_ptr<ResourceStage> const& stage) const { return stage->_id;  }
	};

	struct SetID {
		SetID(int32_t id) : _id{id} {}
		void operator() (std::shared_ptr<RootDummyStage> const& stage) const { stage->_id = _id; }
		void operator() (std::shared_ptr<DrawStage> const& stage) const { stage->_id = _id; }
		void operator() (std::shared_ptr<ComputeStage> const& stage) const { stage->_id = _id; }
		void operator() (std::shared_ptr<ResourceStage> const& stage) const { stage->_id = _id; }
	private:
		int32_t _id;
	};

	struct GetName {
		char const* operator() (std::shared_ptr<RootDummyStage> const& stage) const { return stage->_name.c_str(); }
		char const* operator() (std::shared_ptr<DrawStage> const& stage) const { return stage->_name.c_str(); }
		char const* operator() (std::shared_ptr<ComputeStage> const& stage) const { return stage->_name.c_str(); }
		char const* operator() (std::shared_ptr<ResourceStage> const& stage) const { return stage->_name.c_str(); }
	};

	struct GetStageName {
		char const* operator() (std::shared_ptr<RootDummyStage> const& stage) const { return stage->_stageName.c_str(); }
		char const* operator() (std::shared_ptr<DrawStage> const& stage) const { return stage->_stageName.c_str(); }
		char const* operator() (std::shared_ptr<ComputeStage> const& stage) const { return stage->_stageName.c_str(); }
		char const* operator() (std::shared_ptr<ResourceStage> const& stage) const { return stage->_stageName.c_str(); }
	};

	struct RenderNode {
		void operator() (std::shared_ptr<RootDummyStage> const& stage) const { stage->RenderNode();  }
		void operator() (std::shared_ptr<DrawStage> const& stage) const { stage->RenderNode(); }
		void operator() (std::shared_ptr<ComputeStage> const& stage) const { stage->RenderNode(); }
		void operator() (std::shared_ptr<ResourceStage> const& stage) const { stage->RenderNode(); }
	};

	struct Expose {
		Expose(int32_t attribute_id = 0) : _attribute{attribute_id} {}
		void operator() (std::shared_ptr<RootDummyStage> const& stage) const {}
		void operator() (std::shared_ptr<DrawStage> const& stage) const {}
		ID3D11Resource* operator() (std::shared_ptr<ComputeStage> const& stage) const { return stage->Expose(_attribute); }
		ID3D11Resource* operator() (std::shared_ptr<ResourceStage> const& stage) const { return stage->Expose(); }
	private:
		int32_t _attribute;
	};
	
	struct Consume {
		Consume(ID3D11Resource* resource, int32_t attribute_id) : _resource{resource}, _attribute{attribute_id} {}
		void operator() (std::shared_ptr<RootDummyStage> const& stage) const {}
		void operator() (std::shared_ptr<DrawStage> const& stage) const {}
		void operator() (std::shared_ptr<ComputeStage> const& stage) const { stage->Consume(_resource, _attribute); }
		void operator() (std::shared_ptr<ResourceStage> const& stage) const {}
	private:
		ID3D11Resource* _resource;
		int32_t _attribute;
	};
}
