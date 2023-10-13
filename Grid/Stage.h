#pragma once

#include <vector>
#include <memory>
#include <variant>

#include "ObjectRenderStage.h"
#include "ComputeStage.h"

namespace Stage {
	using Stage =
		std::variant<
			std::shared_ptr<ObjectRenderStage>,
			std::shared_ptr<ComputeStage>
		>;

	struct Run {
		Run(ID3D11DeviceContext& context) : _context{ context } {}
		void operator() (std::shared_ptr<ObjectRenderStage> stage) { stage->Run(_context); }
		void operator() (std::shared_ptr<ComputeStage> stage) { stage->Run(_context); }
	private:
		ID3D11DeviceContext& _context;
	};

	struct AddChild {
		AddChild(int32_t child_id) : _childID{child_id} {}
		void operator() (std::shared_ptr<ObjectRenderStage> stage) { stage->_child.emplace_back(_childID);  }
		void operator() (std::shared_ptr<ComputeStage> stage) { stage->_child.emplace_back(_childID);  }
	private:
		int32_t _childID;
	};

	struct GetChilds {
		std::vector<int32_t> const& operator() (std::shared_ptr<ObjectRenderStage> stage) { return stage->_child; }
		std::vector<int32_t> const& operator() (std::shared_ptr<ComputeStage> stage) { return stage->_child; }
	};

	struct SetParent {
		SetParent(int32_t parent_id) : _parentID{parent_id} {}
		void operator() (std::shared_ptr<ObjectRenderStage> stage) { stage->_parent = _parentID; }
		void operator() (std::shared_ptr<ComputeStage> stage) { stage->_parent = _parentID; }
	private:
		int32_t _parentID;
	};

	struct GetParent {
		int32_t operator() (std::shared_ptr<ObjectRenderStage> stage) { return stage->_parent;  }
		int32_t operator() (std::shared_ptr<ComputeStage> stage) { return stage->_parent;  }
	};

	struct SetID {
		SetID(int32_t id) : _id{id} {}
		void operator() (std::shared_ptr<ObjectRenderStage> stage) { stage->_id = _id; }
		void operator() (std::shared_ptr<ComputeStage> stage) { stage->_id = _id; }
	private:
		int32_t _id;
};
}
