#pragma once

#include "ResourceStage.h"

class Velocity2DStage final : public ResourceStage
{
public: Velocity2DStage();
public: ~Velocity2DStage() override;
public: ID3D11Resource* Expose(int32_t attribute_id = 0) const override;
public: void RenderNode() const override;
private:
	Microsoft::WRL::ComPtr<ID3D11Texture2D> _resource;
};

