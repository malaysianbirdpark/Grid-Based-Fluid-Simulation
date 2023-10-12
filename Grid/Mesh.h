#pragma once

#include <vector>

template <typename VertexType>
class Mesh {
public:
	Mesh(ID3D11Device& device, std::vector<VertexType> const& vertices, std::vector<uint16_t> const& indices);

    UINT GetCount() const;

    void Bind(ID3D11DeviceContext& context);
private:
	UINT                                   _indexCount;
	Microsoft::WRL::ComPtr<ID3D11Buffer>   _indexBuffer;

	D3D11_PRIMITIVE_TOPOLOGY               _topology;
	std::vector<D3D11_INPUT_ELEMENT_DESC>  _inputElementDesc{};
	Microsoft::WRL::ComPtr<ID3D11Buffer>   _vertexBuffer;
};

template<typename VertexType>
inline Mesh<VertexType>::Mesh(ID3D11Device& device, std::vector<VertexType> const& vertices, std::vector<uint16_t> const& indices)
{
    // vertex buffer
    {
        D3D11_BUFFER_DESC bd{};
        bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
        bd.Usage = D3D11_USAGE_IMMUTABLE;
        bd.CPUAccessFlags = 0u;
        bd.MiscFlags = 0u;
        bd.ByteWidth = static_cast<UINT>(sizeof(VertexType) * vertices.size());
        bd.StructureByteStride = sizeof(VertexType);

        D3D11_SUBRESOURCE_DATA sd{};
        sd.pSysMem = vertices.data();

        device.CreateBuffer(&bd, &sd, _vertexBuffer.ReleaseAndGetAddressOf());
    }

    // index buffer
    {
        _indexCount = indices.size();

        D3D11_BUFFER_DESC bd{};
        bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
        bd.Usage = D3D11_USAGE_IMMUTABLE;
        bd.CPUAccessFlags = 0u;
        bd.MiscFlags = 0u;
        bd.ByteWidth = static_cast<UINT>(_indexCount * sizeof(uint16_t));
        bd.StructureByteStride = sizeof(uint16_t);

        D3D11_SUBRESOURCE_DATA sd{};
        sd.pSysMem = indices.data();

        device.CreateBuffer(&bd, &sd, _indexBuffer.ReleaseAndGetAddressOf());
    }

    _topology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
    _inputElementDesc = {
        {"POSITION", 0u, DXGI_FORMAT_R32G32B32_FLOAT, 0u, 0u, D3D11_INPUT_PER_VERTEX_DATA, 0u},
        {"NORMAL", 0u, DXGI_FORMAT_R32G32B32_FLOAT, 0u, sizeof(DirectX::XMFLOAT3), D3D11_INPUT_PER_VERTEX_DATA, 0u}
    };
}

template<typename VertexType>
inline UINT Mesh<VertexType>::GetCount() const
{
    return _indexCount;
}

template<typename VertexType>
inline void Mesh<VertexType>::Bind(ID3D11DeviceContext& context)
{
    context.IASetPrimitiveTopology(_topology);
    static UINT constexpr offset{ 0u };
    static UINT constexpr stride{ sizeof(VertexType) };
    context.IASetVertexBuffers(0u, 1u, _vertexBuffer.GetAddressOf(), &stride, &offset);
	context.IASetIndexBuffer(_indexBuffer.Get(), DXGI_FORMAT_R16_UINT, 0u);
}
