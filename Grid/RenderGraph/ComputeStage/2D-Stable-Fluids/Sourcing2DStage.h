#pragma once

#include "ComputeStage.h"

class Sourcing2DStage final : public ComputeStage
{
public: Sourcing2DStage();
public: void Run(ID3D11DeviceContext& context) override;
public: virtual ~Sourcing2DStage() override = default;
public: void Consume(ID3D11Resource* resource, int32_t attribute_id) override;
public: ID3D11Resource* Expose(int32_t attribute_id) override;
private:
	int32_t _fluidColorID{};
	int32_t _velocityInID{};
	int32_t _quantityInID{};
	int32_t _velocityOutID{};
	int32_t _quantityOutID{};
};

