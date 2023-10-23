#pragma once

#include <memory>

#include "RenderObject.h"
#include "Mesh.h"

class VolumeCube final : public RenderObject
{
	struct Vertex {
		DirectX::XMFLOAT3 _pos{};
		DirectX::XMFLOAT3 _uv{};
	};
public: explicit VolumeCube(ID3D11DeviceContext& context);
public: void     Update(ID3D11DeviceContext& context, float const dt) override;
public: void     Draw(ID3D11DeviceContext& context) override;
private:
	std::unique_ptr<class PipelineStateObject> _pso;
	std::unique_ptr<class Transform>           _transform;
	std::unique_ptr<Mesh<Vertex>>              _mesh;
};

