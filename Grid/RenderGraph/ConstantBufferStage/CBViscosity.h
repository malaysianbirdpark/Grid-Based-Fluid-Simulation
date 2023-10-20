#pragma once

#include "ConstantBufferStage.h"

class CBViscosity : public ConstantBufferStage
{
	struct Constants {
		float alpha{};
		float r_beta{};
		float viscosity{ 1.0f };
		float dt;
	};

public: CBViscosity(ID3D11DeviceContext& context);
public: ~CBViscosity() override = default;

public: virtual void Run(ID3D11DeviceContext& context) override;
public: virtual void Upload(ID3D11DeviceContext& context) override;
public: virtual void Update() override;

private: int32_t   _csID{};
private: Constants _data{};
};

