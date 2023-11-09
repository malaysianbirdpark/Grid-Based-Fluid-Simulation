#include "pch.h"
#include "Transform.h"

#include "Camera.h"
#include "Renderer.h"

Transform::Transform(ID3D11DeviceContext& context, DirectX::FXMMATRIX model)
{
    D3D11_BUFFER_DESC bd{};
    bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    bd.Usage = D3D11_USAGE_DYNAMIC;
    bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    bd.MiscFlags = 0;
    bd.ByteWidth = sizeof(Data);
    bd.StructureByteStride = 0u;

    using namespace DirectX;

    XMStoreFloat4x4(&_model, model);

    D3D11_SUBRESOURCE_DATA sd{};
    XMStoreFloat4x4(&_transform._m, XMMatrixTranspose(model));
    XMStoreFloat4x4(&_transform._mit, XMMatrixInverse(nullptr, model));
    XMStoreFloat4x4(&_transform._mvp, XMMatrixTranspose(XMMatrixMultiply(XMMatrixMultiply(model, Camera::GetView()), Renderer::GetProj())));
    sd.pSysMem = &_transform;

    pDevice->CreateBuffer(&bd, &sd, _resource.ReleaseAndGetAddressOf());
    Update(context);
}

void Transform::Accumulate(DirectX::FXMMATRIX transform)
{
    DirectX::XMStoreFloat4x4(&_model, DirectX::XMMatrixMultiply(DirectX::XMLoadFloat4x4(&_model), transform));
}

void Transform::SetModel(DirectX::FXMMATRIX model)
{
    DirectX::XMStoreFloat4x4(&_model, model);
}

void Transform::Update(ID3D11DeviceContext& context)
{
    using namespace DirectX;
    auto const model{ XMLoadFloat4x4(&_model) };
    XMStoreFloat4x4(&_transform._m, XMMatrixTranspose(model));
    XMStoreFloat4x4(&_transform._mit, XMMatrixInverse(nullptr, model));
    XMStoreFloat4x4(&_transform._mvp, XMMatrixTranspose(XMMatrixMultiply(XMMatrixMultiply(model, Camera::GetView()), Renderer::GetProj())));

    D3D11_MAPPED_SUBRESOURCE msr{};
    context.Map(
        _resource.Get(),
        0u,
        D3D11_MAP_WRITE_DISCARD,
        0u,
        &msr
    );
    memcpy(msr.pData, &_transform, sizeof(Data));
    context.Unmap(_resource.Get(), 0u);
}

void Transform::Bind(ID3D11DeviceContext& context) const
{
    context.VSSetConstantBuffers(0u, 1u, _resource.GetAddressOf());
    context.CSSetConstantBuffers(2u, 1u, _resource.GetAddressOf());
}

InverseTransform::InverseTransform(ID3D11DeviceContext& context)
{
    auto const model{ DirectX::XMMatrixIdentity() };

    D3D11_BUFFER_DESC bd{};
    bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    bd.Usage = D3D11_USAGE_DYNAMIC;
    bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    bd.MiscFlags = 0;
    bd.ByteWidth = sizeof(Data);
    bd.StructureByteStride = 0u;

    using namespace DirectX;

    XMStoreFloat4x4(&_model, model);

    D3D11_SUBRESOURCE_DATA sd{};
    XMStoreFloat4x4(&_transform._mip, XMMatrixTranspose(XMMatrixMultiply(XMMatrixInverse(nullptr, model), Renderer::GetProj())));
    sd.pSysMem = &_transform;

    pDevice->CreateBuffer(&bd, &sd, _resource.ReleaseAndGetAddressOf());
    Update(context);
}

void InverseTransform::SetModel(DirectX::FXMMATRIX model)
{
    DirectX::XMStoreFloat4x4(&_model, model);
}

void InverseTransform::SetProj(DirectX::FXMMATRIX proj)
{
    DirectX::XMStoreFloat4x4(&_proj, proj);
}

void InverseTransform::Update(ID3D11DeviceContext& context)
{
    using namespace DirectX;
    auto const model{ XMLoadFloat4x4(&_model) };
    auto const proj{ XMLoadFloat4x4(&_proj) };
    static auto view {DirectX::XMMatrixLookToLH({0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 1.0f}, {0.0f, 1.0f, 0.0f})};
    XMStoreFloat4x4(
        &_transform._mip, 
        XMMatrixTranspose(
            XMMatrixMultiply(
                XMMatrixInverse(nullptr, model), 
                XMMatrixMultiply(
                    view,
                    proj
                )
            )
        )
    );

    D3D11_MAPPED_SUBRESOURCE msr{};
    context.Map(
        _resource.Get(),
        0u,
        D3D11_MAP_WRITE_DISCARD,
        0u,
        &msr
    );
    memcpy(msr.pData, &_transform, sizeof(Data));
    context.Unmap(_resource.Get(), 0u);
}

void InverseTransform::Bind(ID3D11DeviceContext& context) const
{
    context.VSSetConstantBuffers(1u, 1u, _resource.GetAddressOf());
}
