#pragma once

#include "Compute3DStage.h"

class PressureProjection3DStage final : public Compute3DStage
{
public: PressureProjection3DStage();
public: ~PressureProjection3DStage() override = default;
public: void Run(ID3D11DeviceContext& context) override;
public: void Consume(ID3D11Resource* resource, int32_t attribute_id) override;
public: ID3D11Resource* Expose(int32_t attribute_id) override;

private:
	int32_t _xInID{};
	int32_t _subTargetID{};
	int32_t _resultID{};
};

