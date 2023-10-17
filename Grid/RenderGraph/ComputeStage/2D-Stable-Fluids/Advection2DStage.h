#pragma once

#include "ComputeStage.h"

class Advection2DStage : public ComputeStage
{
public: Advection2DStage();
public: ~Advection2DStage() override;
public: void Consume(ID3D11Resource* resource, int32_t attribute_id) override;
public: ID3D11Resource* Expose(int32_t attribute_id) override;
private:
	int32_t _velocityInID{};
	int32_t _velocityOutID{};
	int32_t _quantityInID{};
	int32_t _quantityOutID{};
};

