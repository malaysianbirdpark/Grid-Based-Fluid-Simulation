#pragma once

#include "ComputeStage.h"

class Divergence2DStage final : public ComputeStage
{
public: Divergence2DStage();
public: ~Divergence2DStage() override = default;
public: void Run(ID3D11DeviceContext& context) override;
public: void Consume(ID3D11Resource* resource, int32_t attribute_id) override;
public: ID3D11Resource* Expose(int32_t attribute_id) override;
private:
	int32_t _xInID{};
	int32_t _xOutID{};
};

