#pragma once

#include <memory>
#include "Mesh.h"

class Sphere {
	struct Vertex {
		DirectX::XMFLOAT3 _pos{};
		DirectX::XMFLOAT3 _normal{};
	};
public:
	explicit Sphere(ID3D11Device& device);
	void Draw(ID3D11DeviceContext& context);
	void DrawInstanced();
private:
	std::unique_ptr<class PipelineStateObject> _pso;
	std::unique_ptr<Mesh<Vertex>> _mesh;
};

