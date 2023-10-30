#pragma once

#include "Compute3DStage.h"

class Diffusion3DStage final : public Compute3DStage
{
public: Diffusion3DStage();
public: virtual ~Diffusion3DStage() override = default;
public: void Run(ID3D11DeviceContext& context) override;
public: void Consume(ID3D11Resource* resource, int32_t attribute_id) override;
public: ID3D11Resource* Expose(int32_t attribute_id) override;
private:
	int32_t _xInID{};
	int32_t _constsID{};
	int32_t _xOutID{};
};

