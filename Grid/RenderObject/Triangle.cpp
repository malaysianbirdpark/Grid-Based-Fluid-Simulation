#include "pch.h"
#include "Triangle.h"

#include "PipelineStateObject.h"
#include "Transform.h"

Triangle::Triangle(ID3D11DeviceContext& context)
{
	std::vector<Vertex> vertices{};
	vertices.push_back({ { -0.5f, 0.0f, 0.0f } });
	vertices.push_back({ { 0.5f, 0.0f, 0.0f } });
	vertices.push_back({ { 0.0f, 0.5f, 0.0f } });

	std::vector<uint16_t> indices{ 0, 2, 1 };

	_mesh = std::make_unique<Mesh<Vertex>>(vertices, indices);

    std::vector<D3D11_INPUT_ELEMENT_DESC> inputElementDesc {
        {"POSITION", 0u, DXGI_FORMAT_R32G32B32_FLOAT, 0u, 0u, D3D11_INPUT_PER_VERTEX_DATA, 0u},
    };
	
	_pso = std::make_unique<PipelineStateObject>();
	_pso->SetVertexShader("./CSO/testVS.cso");
	_pso->SetInputLayout(inputElementDesc);
	_pso->SetPixelShader("./CSO/testPS.cso");

	_transform = std::make_unique<Transform>(context, DirectX::XMMatrixIdentity());
	_transform->SetModel(DirectX::XMMatrixScaling(10.0f, 10.0f, 10.0f));
}

void Triangle::Update(ID3D11DeviceContext& context, float const dt)
{
	_transform->Update(context);
}

void Triangle::Draw(ID3D11DeviceContext& context)
{
	_transform->Bind(context);
	_pso->Bind(context);
	_mesh->Bind(context);
	context.DrawIndexed(_mesh->GetCount(), 0u, 0u);
}

void Triangle::DrawInstanced()
{
}
