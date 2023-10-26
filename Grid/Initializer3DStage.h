#pragma once

#include "Compute3DStage.h"

class Initializer3DStage final : public Compute3DStage
{
public: Initializer3DStage();
public: virtual ~Initializer3DStage() override = default;
public: void Consume(ID3D11Resource* resource, int32_t attribute_id) override;
public: ID3D11Resource* Expose(int32_t attribute_id) override;
private:
    int32_t _targetID{};
};


