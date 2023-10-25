#pragma once

#include <memory>

#include "RenderObject.h"
#include "Mesh.h"

class VolumeCube final : public RenderObject
{
	struct Vertex {
		DirectX::XMFLOAT3 _pos{};
		DirectX::XMFLOAT3 _uvw{};
	};
public: explicit VolumeCube(ID3D11DeviceContext& context);
public: void     Update(ID3D11DeviceContext& context, float const dt) override;
public: void     Draw(ID3D11DeviceContext& context) override;
public: std::vector<D3D11_INPUT_ELEMENT_DESC> const& GetInputElementDest() const override;
private:
	std::unique_ptr<class Transform>           _transform;
	std::unique_ptr<Mesh<Vertex>>              _mesh;
	inline static std::vector<D3D11_INPUT_ELEMENT_DESC> _inputElementDesc {
        {"POSITION", 0u, DXGI_FORMAT_R32G32B32_FLOAT, 0u, 0u, D3D11_INPUT_PER_VERTEX_DATA, 0u},
        {"TEXCOORD", 0u, DXGI_FORMAT_R32G32B32_FLOAT, 0u, sizeof(DirectX::XMFLOAT3), D3D11_INPUT_PER_VERTEX_DATA, 0u},
    };
};

