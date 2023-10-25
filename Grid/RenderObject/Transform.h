#pragma once
class Transform
{
    struct Data {
        DirectX::XMFLOAT4X4 _m;
        DirectX::XMFLOAT4X4 _mv;
        DirectX::XMFLOAT4X4 _mvi;
        DirectX::XMFLOAT4X4 _mvp;
    };
public:
    Transform(
        ID3D11DeviceContext& context,
        DirectX::FXMMATRIX model
    );

    void Accumulate(DirectX::FXMMATRIX transform);
    void SetModel(DirectX::FXMMATRIX model);

    void Update(ID3D11DeviceContext& context);
    void Bind(ID3D11DeviceContext& context) const;
private:
    Microsoft::WRL::ComPtr<ID3D11Buffer> _resource;
    DirectX::XMFLOAT4X4 _model{};
    Data _transform{};
};

