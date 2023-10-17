#pragma once

#include "ConstantBufferStage.h"

class CBFluidColor final : public ConstantBufferStage
{
    struct Constants {
		DirectX::XMFLOAT4 _color;
	};

public: CBFluidColor();
public: ~CBFluidColor() override = default;

public: virtual void Run(ID3D11DeviceContext& context) override;
public: virtual void Upload(ID3D11DeviceContext& context) override;
public: virtual void Update() override;

private: Constants _data;
private: int32_t _csID{};
};

