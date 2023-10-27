#pragma once

#include "Compute3DStage.h"

class Poisson3D3DStage final : public Compute3DStage
{
public: Poisson3D3DStage();
public: virtual ~Poisson3D3DStage() override = default;
public: void Run(ID3D11DeviceContext& context) override;
public: void Consume(ID3D11Resource* resource, int32_t attribute_id) override;
public: ID3D11Resource* Expose(int32_t attribute_id) override;
private:
	int32_t _xInID{};
	int32_t _bID{};
	int32_t _constID{};
	int32_t _xOutID{};
};

