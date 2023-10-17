#pragma once

#include "ComputeStage.h"

class Sourcing2DStage final : public ComputeStage
{
public: Sourcing2DStage();
public: ~Sourcing2DStage() override;
public: void Consume(ID3D11Resource* resource, int32_t attribute_id) override;
public: ID3D11Resource* Expose(int32_t attribute_id) override;
private:
	int32_t _fluidColorID{};
	int32_t _velocityID{};
	int32_t _quantityID{};
};

