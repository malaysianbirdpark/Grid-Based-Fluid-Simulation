#pragma once

#include "ConstantBufferStage.h"

class CBPoisson final : public ConstantBufferStage
{
	struct Constants {
		float alpha{};
		float r_beta{};
		float pad[2]{};
	};

public: CBPoisson(ID3D11DeviceContext& context, float alpha, float beta);
public: ~CBPoisson() override = default;

public: virtual void Run(ID3D11DeviceContext& context) override;
public: virtual void Upload(ID3D11DeviceContext& context) override;
public: virtual void Update() override;

private: Constants _data;
private: int32_t _csID{};
};

