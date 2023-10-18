#pragma once

#include "ResourceStage.h"

class Pressure2DStage final : public ResourceStage
{
public: Pressure2DStage();
public: ~Pressure2DStage() override = default;
public: ID3D11Resource* Expose(int32_t attribute_id = 0) const override;
private:
	Microsoft::WRL::ComPtr<ID3D11Texture2D> _resource;
};

