#pragma once

#include "Compute3DStage.h"

class Advection3DStage final : public Compute3DStage
{
public: Advection3DStage();
public: virtual ~Advection3DStage() override = default;
public: void Run(ID3D11DeviceContext& context) override;
public: void Consume(ID3D11Resource* resource, int32_t attribute_id) override;
public: ID3D11Resource* Expose(int32_t attribute_id) override;
private:
	int32_t _cbTimestepID{};
	int32_t _velocityInID{};
	int32_t _velocityOutID{};
	int32_t _quantityInID{};
	int32_t _quantityOutID{};
};
