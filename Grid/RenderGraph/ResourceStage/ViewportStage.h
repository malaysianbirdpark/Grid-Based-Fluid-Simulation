#pragma once

#include "ResourceStage.h"

class ViewportStage final : public ResourceStage
{
public: explicit ViewportStage();
public: ~ViewportStage() override = default;
public: ID3D11Resource* Expose(int32_t attribute_id) const override;
};

