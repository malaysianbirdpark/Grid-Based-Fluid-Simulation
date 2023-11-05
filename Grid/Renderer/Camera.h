#pragma once
class Camera
{
    struct Constants {
        DirectX::XMFLOAT3 _pos;
        float             _pad;
    };
public:
    static void Init();

    static DirectX::XMVECTOR GetPos();
    static DirectX::XMMATRIX GetView();

    static void Run(ID3D11DeviceContext& context);
    static void Reset();

    static void Rotate(float const dx, float const dy);
    static void Translate(DirectX::XMFLOAT3 const& vec);
private:
    inline static Constants                            _data;
    inline static Microsoft::WRL::ComPtr<ID3D11Buffer> _buffer;
    inline static float _pitch{ 0.0f };
    inline static float _yaw{ 0.0f };
    inline static float constexpr _travelSpeed{ 6.5f };
    inline static float constexpr _rotationSpeed{ 0.55f };
};

