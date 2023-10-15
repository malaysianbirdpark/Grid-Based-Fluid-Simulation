#pragma once

#include <memory>

#include "Mesh.h"

class Triangle {
	struct Vertex {
		DirectX::XMFLOAT3 _pos{};
	};
public:
	explicit Triangle(ID3D11Device& device, ID3D11DeviceContext& context);
	void Update(ID3D11DeviceContext& context, float const dt);
	void Draw(ID3D11DeviceContext& context);
	void DrawInstanced();
private:
	std::unique_ptr<class PipelineStateObject> _pso;
	std::unique_ptr<class Transform> _transform;
	std::unique_ptr<Mesh<Vertex>> _mesh;

};

