#pragma once

#include "Compute3DStage.h"

class GradientSubtract3DStage final : public Compute3DStage
{
public: GradientSubtract3DStage();
public: ~GradientSubtract3DStage() override = default;
public: void Consume(ID3D11Resource* resource, int32_t attribute_id) override;
public: ID3D11Resource* Expose(int32_t attribute_id) override;
private:
	int32_t _xInID{};
	int32_t _subTargetID{};
	int32_t _resultID{};
};

