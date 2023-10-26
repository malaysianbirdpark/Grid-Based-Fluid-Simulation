#pragma once

#include "ResourceStage.h"

class Pressure2D1DStage final : public ResourceStage
{
public: Pressure2D1DStage();
public: ~Pressure2D1DStage() override = default;
public: ID3D11Resource* Expose(int32_t attribute_id = 0) const override;
private:
	Microsoft::WRL::ComPtr<ID3D11Texture2D> _resource;
};

