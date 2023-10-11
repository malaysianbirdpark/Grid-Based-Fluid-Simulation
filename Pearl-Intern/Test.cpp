#include "pch.h"
#include "Test.h"

#include "PipelineStateObject.h"

Test::Test(ID3D11Device& device)
{
    // vertex buffer
    {
        D3D11_BUFFER_DESC bd{};
        bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
        bd.Usage = D3D11_USAGE_IMMUTABLE;
        bd.CPUAccessFlags = 0u;
        bd.MiscFlags = 0u;
        bd.ByteWidth = sizeof(Vertex) * 1;
        bd.StructureByteStride = sizeof(Vertex);

        std::vector<Vertex> vertices{
            {{0.0f, 0.0f, 0.0f}, {1.0f, 0.0f, 0.0f}}
        };
        D3D11_SUBRESOURCE_DATA sd{};
        sd.pSysMem = vertices.data();

        device.CreateBuffer(&bd, &sd, _vertexBuffer.ReleaseAndGetAddressOf());
    }

    // index buffer
    {
        _indexCount = 1;

        D3D11_BUFFER_DESC bd{};
        bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
        bd.Usage = D3D11_USAGE_IMMUTABLE;
        bd.CPUAccessFlags = 0u;
        bd.MiscFlags = 0u;
        bd.ByteWidth = static_cast<UINT>(_indexCount * sizeof(uint32_t));
        bd.StructureByteStride = sizeof(uint32_t);

        std::vector<uint16_t> indices{ 0 };
        D3D11_SUBRESOURCE_DATA sd{};
        sd.pSysMem = indices.data();

        device.CreateBuffer(&bd, &sd, _indexBuffer.ReleaseAndGetAddressOf());
    }

    _topology = D3D11_PRIMITIVE_TOPOLOGY_POINTLIST;
    _inputElementDesc = {
        {"POSITION", 0u, DXGI_FORMAT_R32G32B32_FLOAT, 0u, 0u, D3D11_INPUT_PER_VERTEX_DATA, 0u},
        {"COLOR", 0u, DXGI_FORMAT_R32G32B32_FLOAT, 0u, sizeof(DirectX::XMFLOAT3), D3D11_INPUT_PER_VERTEX_DATA, 0u}
    };

    _pso = std::make_unique<PipelineStateObject>();
    _pso->SetVertexShader(device, "testVS.cso");
    _pso->SetInputLayout(device, _inputElementDesc);
    _pso->SetGeometryShader(device, "testGS.cso");
    _pso->SetPixelShader(device, "testPS.cso");
}

void Test::Draw(ID3D11DeviceContext& context)
{
    _pso->Bind(context);
    context.DrawIndexed(_indexCount, 0u, 0u);
}
