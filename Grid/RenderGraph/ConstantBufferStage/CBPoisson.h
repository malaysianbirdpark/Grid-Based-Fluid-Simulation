#pragma once

#include "ConstantBufferStage.h"

class CBPoisson final : public ConstantBufferStage
{
	struct Constants {
		float alpha{-1.0f};
		float r_beta{1.0f/6};
		float pad[2]{};
	};

public: CBPoisson(ID3D11DeviceContext& context);
public: ~CBPoisson() override = default;

public: virtual void Run(ID3D11DeviceContext& context) override;
public: virtual void Upload(ID3D11DeviceContext& context) override;
public: virtual void Update() override;

private: Constants _data;
private: int32_t _csID{};
};

