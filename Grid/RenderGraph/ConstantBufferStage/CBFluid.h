#pragma once

#include "ConstantBufferStage.h"

class CBFluid final : public ConstantBufferStage
{
	struct Constants {
		DirectX::XMFLOAT3 _velocityDir;
		float             _speed{ 2.0f };
		DirectX::XMFLOAT4 _color;
		float             _colorScale{ 2.0f };
		float             _dissipation{ 0.00182f };
		float             _pad[2];
	};

public: CBFluid();
public: ~CBFluid() override = default;

public: virtual void Run(ID3D11DeviceContext& context) override;
public: virtual void Upload(ID3D11DeviceContext& context) override;
public: virtual void Update() override;

private: int32_t color_mode{};
private: int32_t velocity_mode{};
private: int32_t _csID{};
private: Constants _data;
};

