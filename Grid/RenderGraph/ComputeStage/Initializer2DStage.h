#pragma once

#include "Compute2DStage.h"
class Initializer2DStage final : public Compute2DStage
{
public: Initializer2DStage();
public: virtual ~Initializer2DStage() override = default;
public: void Consume(ID3D11Resource* resource, int32_t attribute_id) override;
public: ID3D11Resource* Expose(int32_t attribute_id) override;
private:
    int32_t _targetID{};
};

