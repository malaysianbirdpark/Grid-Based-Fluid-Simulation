#include "pch.h"
#include "Camera.h"

#include <algorithm>

#define PI (3.141592f)

void Camera::Init() {
    DirectX::XMVECTOR const init_pos{ 0.0f, 0.0f, -5.0f, 1.0f };
    DirectX::XMStoreFloat3(&_pos, init_pos);
}

DirectX::XMVECTOR Camera::GetPos() {
    return DirectX::XMLoadFloat3(&_pos);
}

DirectX::XMMATRIX Camera::GetView() {
    auto const lookAt{
        DirectX::XMVector3Transform(
        DirectX::XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f),
        DirectX::XMMatrixRotationRollPitchYaw(_pitch, _yaw, 0.0f)
    )
    };
    auto const camPos{
        DirectX::XMLoadFloat3(&_pos)
    };
    // TODO: CamTarget
    auto const camTarget{
        DirectX::XMVectorAdd(camPos, lookAt)
    };
    return DirectX::XMMatrixLookAtLH(
        camPos,
        camTarget,
        DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f)
    );
}

void Camera::Update() {
}

void Camera::Reset() {
    _pos = DirectX::XMFLOAT3{ 0.0f, 0.0f, 0.0f };
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

    _pos = {
        _pos.x + transform.m128_f32[0],
        _pos.y + transform.m128_f32[1],
        _pos.z + transform.m128_f32[2]
    };
}
