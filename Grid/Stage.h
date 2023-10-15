#pragma once

#include <vector>
#include <string>
#include <memory>
#include <variant>

#include "DrawStage.h"
#include "ComputeStage.h"

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
			std::shared_ptr<ComputeStage>
		>;

	struct Run {
		Run(ID3D11DeviceContext& context) : _context{ context } {}
		void operator() (std::shared_ptr<RootDummyStage> stage) const {}
		void operator() (std::shared_ptr<DrawStage> const& stage) const { stage->Run(_context); }
		void operator() (std::shared_ptr<ComputeStage> const& stage) const { stage->Run(_context); }
	private:
		ID3D11DeviceContext& _context;
	};

	struct AddChild {
		AddChild(int32_t child_id) : _childID{child_id} {}
		void operator() (std::shared_ptr<RootDummyStage> const& stage) { stage->_outgoing.emplace_back(_childID); }
		void operator() (std::shared_ptr<DrawStage> const& stage) { stage->_outgoing.emplace_back(_childID);  }
		void operator() (std::shared_ptr<ComputeStage> const& stage) { stage->_outgoing.emplace_back(_childID);  }
	private:
		int32_t _childID;
	};

	struct GetOutgoings {
		std::vector<int32_t> const& operator() (std::shared_ptr<RootDummyStage> const& stage) const { return stage->_outgoing; }
		std::vector<int32_t> const& operator() (std::shared_ptr<DrawStage> const& stage) const { return stage->_outgoing; }
		std::vector<int32_t> const& operator() (std::shared_ptr<ComputeStage> const& stage) const { return stage->_outgoing; }
	};

	struct AddIncoming {
		AddIncoming(int32_t from) : _from{from} {}
		void operator() (std::shared_ptr<RootDummyStage> const& stage) const {}
		void operator() (std::shared_ptr<DrawStage> const& stage) const {}
		void operator() (std::shared_ptr<ComputeStage> const& stage) const {}
	private:
		int32_t _from;
	};

	struct GetIncoming {
		std::vector<int32_t> operator() (std::shared_ptr<RootDummyStage> const& stage) const { return stage->_incoming; }
		std::vector<int32_t> operator() (std::shared_ptr<DrawStage> const& stage) const { return stage->_incoming;  }
		std::vector<int32_t> operator() (std::shared_ptr<ComputeStage> const& stage) const { return stage->_incoming;  }
	};

	struct GetID {
		int32_t operator() (std::shared_ptr<RootDummyStage> const& stage) const { return stage->_id; }
		int32_t operator() (std::shared_ptr<DrawStage> const& stage) const { return stage->_id;  }
		int32_t operator() (std::shared_ptr<ComputeStage> const& stage) const { return stage->_id;  }
	};

	struct SetID {
		SetID(int32_t id) : _id{id} {}
		void operator() (std::shared_ptr<RootDummyStage> const& stage) const { stage->_id = _id; }
		void operator() (std::shared_ptr<DrawStage> const& stage) const { stage->_id = _id; }
		void operator() (std::shared_ptr<ComputeStage> const& stage) const { stage->_id = _id; }
	private:
		int32_t _id;
	};

	struct GetName {
		char const* operator() (std::shared_ptr<RootDummyStage> const& stage) const { return stage->_name.c_str(); }
		char const* operator() (std::shared_ptr<DrawStage> const& stage) const { return stage->_name.c_str(); }
		char const* operator() (std::shared_ptr<ComputeStage> const& stage) const { return stage->_name.c_str(); }
	};

	struct GetStageName {
		char const* operator() (std::shared_ptr<RootDummyStage> const& stage) const { return stage->_stageName.c_str(); }
		char const* operator() (std::shared_ptr<DrawStage> const& stage) const { return stage->_stageName.c_str(); }
		char const* operator() (std::shared_ptr<ComputeStage> const& stage) const { return stage->_stageName.c_str(); }
	};

	struct RenderNode {
		void operator() (std::shared_ptr<RootDummyStage> const& stage) const { stage->RenderNode();  }
		void operator() (std::shared_ptr<DrawStage> const& stage) const { stage->RenderNode(); }
		void operator() (std::shared_ptr<ComputeStage> const& stage) const { stage->RenderNode(); }
	};
}
