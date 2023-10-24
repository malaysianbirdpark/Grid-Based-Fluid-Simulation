#pragma once

#include "Compute3DStage.h"

class Texture3DTest final : public Compute3DStage {
public: Texture3DTest();
public: virtual ~Texture3DTest() override = default;
public: void Run(ID3D11DeviceContext& context) override;
public: void Consume(ID3D11Resource* resource, int32_t attribute_id) override;
public: ID3D11Resource* Expose(int32_t attribute_id) override;
private:
	int32_t _resultID{};
};

