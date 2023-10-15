#pragma once

#include <memory>

#include "RenderObject.h"
#include "Mesh.h"

class Sphere : public RenderObject {
	struct Vertex {
		DirectX::XMFLOAT3 _pos{};
		DirectX::XMFLOAT3 _normal{};
		DirectX::XMFLOAT2 _uv{};
	};
public:
	explicit Sphere(ID3D11DeviceContext& context);
	void Update(ID3D11DeviceContext& context, float const dt) override;
	void Draw(ID3D11DeviceContext& context) override;
	void DrawInstanced();
private:
	std::unique_ptr<class PipelineStateObject> _pso;
	std::unique_ptr<class Transform> _transform;
	std::unique_ptr<class PSTextures> _pstex;
	std::unique_ptr<Mesh<Vertex>> _mesh;
};

