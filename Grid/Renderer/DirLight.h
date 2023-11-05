#pragma once

class DirLight
{
	struct Constants {
		DirectX::XMFLOAT3 _dir{};
		float             _pad{};
		DirectX::XMFLOAT3 _color{};
		float             _intensity{1.0f};
	};

public: DirLight();
public: ~DirLight() = default;

public: virtual void Run(ID3D11DeviceContext& context);
public: virtual void Upload(ID3D11DeviceContext& context);
public: virtual void Update();

private:   Constants _data;
private:   Microsoft::WRL::ComPtr<ID3D11Buffer> _buffer;
};

