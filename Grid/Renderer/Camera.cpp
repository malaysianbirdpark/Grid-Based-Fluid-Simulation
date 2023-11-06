#include "pch.h"
#include "Camera.h"

#include <algorithm>

#define PI (3.141592f)

void Camera::Init() {
    //DirectX::XMVECTOR const init_pos{ 0.5f, 1.7f, -2.2f, 1.0f };
    DirectX::XMVECTOR const init_pos{ 0.0f, 0.0f, 0.0f, 1.0f };
    //_pitch = 0.6f;
    //_yaw = -0.3f;
    _pitch = 0.0f;
    _yaw = 0.0f;
    DirectX::XMStoreFloat3(&_data._pos, init_pos);

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

DirectX::XMVECTOR Camera::GetPos() {
    return DirectX::XMLoadFloat3(&_data._pos);
}

DirectX::XMMATRIX Camera::GetView() {
    auto const lookAt{
        DirectX::XMVector3Transform(
			DirectX::XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f),
			DirectX::XMMatrixRotationRollPitchYaw(_pitch, _yaw, 0.0f)
		)
    };
    auto const camPos{
        DirectX::XMLoadFloat3(&_data._pos)
    };
    auto const camTarget{
        DirectX::XMVectorAdd(camPos, lookAt)
    };
    return DirectX::XMMatrixLookAtLH(
        camPos,
        camTarget,
        DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f)
    );
}

void Camera::Run(ID3D11DeviceContext& context) {
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
	context.PSSetConstantBuffers(0u, 1u, _buffer.GetAddressOf());
}

void Camera::Reset() {
    _data._pos = DirectX::XMFLOAT3{ 0.0f, 0.0f, 0.0f };
    _pitch = 0.0f;
    _yaw = 0.0f;
}

void Camera::Rotate(float const dx, float const dy) {
    _yaw = _yaw + dx * _rotationSpeed;
    _pitch = std::clamp(_pitch + dy * _rotationSpeed, -(PI / 2.0f) * 0.995f, PI / 2.0f * 0.995f);
}

void Camera::Translate(DirectX::XMFLOAT3 const& vec) {
    auto const transform{
        DirectX::XMVector3Transform(
            DirectX::XMVectorScale(DirectX::XMLoadFloat3(&vec), _travelSpeed),
            DirectX::XMMatrixRotationRollPitchYaw(_pitch, _yaw, 0.0f)
        )
    };

    _data._pos = {
        _data._pos.x + transform.m128_f32[0],
        _data._pos.y + transform.m128_f32[1],
        _data._pos.z + transform.m128_f32[2]
    };
}
