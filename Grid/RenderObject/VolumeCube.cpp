#include "pch.h"
#include "VolumeCube.h"

#include "PipelineStateObject.h"
#include "Transform.h"

VolumeCube::VolumeCube(ID3D11DeviceContext& context)
{
	std::vector<Vertex> vertices;
	vertices.resize(8);

	vertices[0] = { {-1.0f,  1.0f, -1.0f}, {0.0f, 0.0f, 0.0f} };
	vertices[1] = { {-1.0f,  1.0f,  1.0f}, {0.0f, 0.0f, 1.0f} };
	vertices[2] = { { 1.0f,  1.0f,  1.0f}, {1.0f, 0.0f, 1.0f} };
	vertices[3] = { { 1.0f,  1.0f, -1.0f}, {1.0f, 0.0f, 0.0f} };
	vertices[4] = { {-1.0f, -1.0f, -1.0f}, {0.0f, 1.0f, 0.0f} };
	vertices[5] = { {-1.0f, -1.0f,  1.0f}, {0.0f, 1.0f, 1.0f} };
	vertices[6] = { { 1.0f, -1.0f,  1.0f}, {1.0f, 1.0f, 1.0f} };
	vertices[7] = { { 1.0f, -1.0f, -1.0f}, {1.0f, 1.0f, 0.0f} };

	std::vector<uint16_t> indices{
		0, 1, 2, 0, 2, 3,
		0, 4, 1, 1, 4, 5,
		0, 4, 3, 3, 4, 7,
		3, 2, 7, 7, 2, 6,
		1, 5, 2, 2, 5, 6,
		7, 6, 5, 4, 7, 5,
	};
	_mesh = std::make_unique<Mesh<Vertex>>(vertices, indices);

    std::vector<D3D11_INPUT_ELEMENT_DESC> inputElementDesc {
        {"POSITION", 0u, DXGI_FORMAT_R32G32B32_FLOAT, 0u, 0u, D3D11_INPUT_PER_VERTEX_DATA, 0u},
        {"TEXCOORD", 0u, DXGI_FORMAT_R32G32B32_FLOAT, 0u, sizeof(DirectX::XMFLOAT3), D3D11_INPUT_PER_VERTEX_DATA, 0u}
    };

	_pso = std::make_unique<PipelineStateObject>();
	_pso->SetVertexShader("./CSO/VolumeCube_VS.cso");
	_pso->SetInputLayout(inputElementDesc);
	_pso->SetPixelShader("./CSO/VolumeCube_PS.cso");

	_transform = std::make_unique<Transform>(context, DirectX::XMMatrixIdentity());
	_transform->SetModel(DirectX::XMMatrixScaling(10.0f, 10.0f, 10.0f));
}

void VolumeCube::Update(ID3D11DeviceContext& context, float const dt)
{
	_transform->Update(context);
}

void VolumeCube::Draw(ID3D11DeviceContext& context)
{
	_transform->Bind(context);
	_pso->Bind(context);
	_mesh->Bind(context);
	context.DrawIndexedInstanced(_mesh->GetCount(), 1u, 0u, 0u, 0u);
}
