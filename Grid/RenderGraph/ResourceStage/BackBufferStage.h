#pragma once

#include "ResourceStage.h"

class BackBufferStage final : public ResourceStage {
public: BackBufferStage(IDXGISwapChain& swap_chain);
public: ~BackBufferStage() override;
public: ID3D11Resource* Expose(int32_t attribute_id) const override;
public: void RenderNode() const override;
};

