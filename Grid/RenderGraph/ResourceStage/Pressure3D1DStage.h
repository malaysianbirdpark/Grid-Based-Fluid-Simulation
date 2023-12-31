#pragma once

#include "ResourceStage.h"

class Pressure3D1DStage final : public ResourceStage
{
public: Pressure3D1DStage();
public: ~Pressure3D1DStage() override = default;
public: ID3D11Resource* Expose(int32_t attribute_id = 0) const override;
private:
	Microsoft::WRL::ComPtr<ID3D11Texture3D> _resource;
};


