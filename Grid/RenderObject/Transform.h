#pragma once
class Transform
{
    struct Data {
        DirectX::XMFLOAT4X4 _m;
        DirectX::XMFLOAT4X4 _mit;
        DirectX::XMFLOAT4X4 _mvp;
    };
public:
    Transform(
        ID3D11DeviceContext& context,
        DirectX::FXMMATRIX model
    );
    ~Transform() = default;

    void Accumulate(DirectX::FXMMATRIX transform);
    void SetModel(DirectX::FXMMATRIX model);

    void Update(ID3D11DeviceContext& context);
    void Bind(ID3D11DeviceContext& context) const;
private:
    Microsoft::WRL::ComPtr<ID3D11Buffer> _resource;
    DirectX::XMFLOAT4X4 _model{};
    Data _transform{};
};

class InverseTransform {
    struct Data {
        DirectX::XMFLOAT4X4 _mip;
    };

public: InverseTransform(ID3D11DeviceContext& context);
public: ~InverseTransform() = default;

public: void SetModel(DirectX::FXMMATRIX model);
public: void SetProj(DirectX::FXMMATRIX proj);

public: void Update(ID3D11DeviceContext& context);
public: void Bind(ID3D11DeviceContext& context) const;
private:
    Microsoft::WRL::ComPtr<ID3D11Buffer> _resource;
    DirectX::XMFLOAT4X4 _model{};
    DirectX::XMFLOAT4X4 _proj{};
    Data _transform{};
};
