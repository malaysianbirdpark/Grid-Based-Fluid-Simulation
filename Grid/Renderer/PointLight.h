#pragma once

class PointLight
{
	struct Constants {
		DirectX::XMFLOAT3 _pos{};
		float             _pad{};
		DirectX::XMFLOAT3 _color{};
		float             _reciprocal_range{};
	};

public: PointLight();
public: ~PointLight() = default;

public: virtual void Run(ID3D11DeviceContext& context);
public: virtual void Upload(ID3D11DeviceContext& context);
public: virtual void Update();

private:   Constants _data;
private:   Microsoft::WRL::ComPtr<ID3D11Buffer> _buffer;
};

