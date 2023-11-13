#include "pch.h"
#include "PointLight.h"

PointLight::PointLight() {
    DirectX::XMStoreFloat3(&_data._pos, DirectX::XMVECTOR{2.0f, 5.0f, 1.0f});
    DirectX::XMStoreFloat3(&_data._color, DirectX::XMVECTOR{1.0f, 1.0f, 1.0f});
    _data._reciprocal_range = 1.0f / 50.0f;

    D3D11_BUFFER_DESC bd{};
    bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    bd.Usage = D3D11_USAGE_DYNAMIC;
    bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    bd.MiscFlags = 0;
    bd.ByteWidth = sizeof(Constants);
    bd.StructureByteStride = 0u;

    D3D11_SUBRESOURCE_DATA sd{};
    sd.pSysMem = &_data;

    pDevice->CreateBuffer(&bd, &sd, _buffer.ReleaseAndGetAddressOf());
}

void PointLight::Run(ID3D11DeviceContext& context) {
    Update();
    Upload(context);
	context.PSSetConstantBuffers(2u, 1u, _buffer.GetAddressOf());
}

void PointLight::Upload(ID3D11DeviceContext& context) {
    D3D11_MAPPED_SUBRESOURCE msr{};
    context.Map(
        _buffer.Get(),
        0u,
        D3D11_MAP_WRITE_DISCARD,
        0u,
        &msr
    );
    memcpy(msr.pData, &_data, sizeof(Constants));
    context.Unmap(_buffer.Get(), 0u);
}

void PointLight::Update() {
}

