#pragma once

#include "ComputeStage.h"

class TestCS final : public ComputeStage {
public: TestCS();
public: ~TestCS() override;
public: void            Consume(ID3D11Resource* resource, int32_t attribute_id) override;
public: ID3D11Resource* Expose(int32_t attribute_id) override;
public: void            RenderNode() const override;
};

