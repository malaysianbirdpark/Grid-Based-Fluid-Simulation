#pragma once

#include "ConstantBufferStage.h"

class CBTimestep final : public ConstantBufferStage
{
	struct Constants {
		float _dt;
		float pad[3];
	};

public: CBTimestep();
public: ~CBTimestep() override = default;

public: virtual void Run(ID3D11DeviceContext& context) override;
public: virtual void Upload(ID3D11DeviceContext& context) override;
public: virtual void Update() override;

private: Constants _data;
private: int32_t _csID{};
};

