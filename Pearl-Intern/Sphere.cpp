#include "pch.h"
#include "Sphere.h"

#include <vector>
#include <memory>

#include "PipelineStateObject.h"

Sphere::Sphere(ID3D11Device& device)
{
	static float constexpr PI{ 3.141592f };
	static float constexpr R{ 1.0f };
	static float constexpr R_inverse{ 1.0f / R };

	static int constexpr sector_count {10};
	static int constexpr stack_count {10};

	static float constexpr sector_step{2 * PI / sector_count};
	static float constexpr stack_step{ PI / stack_count };

	std::vector<Vertex> vertices;
	vertices.reserve(stack_count * sector_count);

	float theta{ 0.0f };
	float phi{ 0.0f };
	for (auto i{ 0 }; i != stack_count; ++i) {
		phi = PI / 2 - i * stack_step;
		float const rcos = R * cosf(phi);
		float const z = R * sinf(phi);

		for (auto j{ 0 }; j != sector_count; ++j) {
			theta = j * sector_step;

			float const x {rcos * cosf(theta)};
			float const y {rcos * sinf(theta)};

			float const nx{ x * R_inverse };
			float const ny{ y * R_inverse };
			float const nz{ z * R_inverse };

			vertices.push_back({ {x, y, z}, {nx, ny, nz} });
		}
	}

    std::vector<uint16_t> indices;
	indices.reserve(3 * stack_count * sector_count);

	for (int i{ 0 }; i != stack_count; ++i) {
		float k0{ i * static_cast<float>(sector_count) + 1 };
		float k1{ k0 + static_cast<float>(sector_count) + 1 };

		for (auto j{ 0 }; j != sector_count; ++j, ++k0, ++k1) {
			if (i != 0) {
				indices.emplace_back(k0);
				indices.emplace_back(k1);
				indices.emplace_back(k1 + 1);
			}

			if (i != stack_count - 1) {
				indices.emplace_back(k0 + 1);
				indices.emplace_back(k1);
				indices.emplace_back(k1 + 1);
			}
		}
	}
	_mesh = std::make_unique<Mesh<Vertex>>(device, vertices, indices);

    std::vector<D3D11_INPUT_ELEMENT_DESC> inputElementDesc {
        {"POSITION", 0u, DXGI_FORMAT_R32G32B32_FLOAT, 0u, 0u, D3D11_INPUT_PER_VERTEX_DATA, 0u},
        {"COLOR", 0u, DXGI_FORMAT_R32G32B32_FLOAT, 0u, sizeof(DirectX::XMFLOAT3), D3D11_INPUT_PER_VERTEX_DATA, 0u}
    };
	
	_pso = std::make_unique<PipelineStateObject>();
	_pso->SetVertexShader(device, "testVS.cso");
	_pso->SetInputLayout(device, inputElementDesc);
	_pso->SetPixelShader(device, "testPS.cso");
}

void Sphere::Draw(ID3D11DeviceContext& context)
{
	_pso->Bind(context);
	_mesh->Bind(context);
	context.DrawIndexed(_mesh->GetCount(), 0u, 0u);
}

void Sphere::DrawInstanced()
{
}
