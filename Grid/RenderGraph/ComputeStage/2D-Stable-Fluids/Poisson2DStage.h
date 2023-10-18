#pragma once

#include "ComputeStage.h"

class Poisson2DStage final : public ComputeStage
{
public: Poisson2DStage();
public: ~Poisson2DStage() override;
public: void Run(ID3D11DeviceContext& context) override;
public: void Consume(ID3D11Resource* resource, int32_t attribute_id) override;
public: ID3D11Resource* Expose(int32_t attribute_id) override;
private:
	int32_t _xInID{};
	int32_t _bID{};
	int32_t _constID{};
	int32_t _xOutID{};
};

