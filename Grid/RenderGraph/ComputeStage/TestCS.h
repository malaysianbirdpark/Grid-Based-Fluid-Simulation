#pragma once

#include "ComputeStage.h"

class TestCS final : public ComputeStage {
public:
    TestCS();
    ~TestCS() override;
    void            Consume(ID3D11Resource* resource, int32_t attribute_id) override;
    ID3D11Resource* Expose(int32_t attribute_id) override;
    void            RenderNode() const override;
};

