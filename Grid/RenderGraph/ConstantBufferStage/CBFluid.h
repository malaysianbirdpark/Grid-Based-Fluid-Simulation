#pragma once

#include "ConstantBufferStage.h"

class CBFluid final : public ConstantBufferStage
{
	struct Constants {
		DirectX::XMFLOAT2 _velocityDir;
		float             _colorScale{ 0.04f };
		float             _speed{ 0.0065f };
		DirectX::XMFLOAT4 _color;
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

