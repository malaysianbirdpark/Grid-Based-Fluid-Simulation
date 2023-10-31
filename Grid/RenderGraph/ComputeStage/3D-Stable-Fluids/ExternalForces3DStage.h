#pragma once

#include "Compute3DStage.h"

class ExternalForces3DStage : public Compute3DStage {
public: ExternalForces3DStage();
public: virtual ~ExternalForces3DStage() override = default;
public: void Run(ID3D11DeviceContext& context) override;
public: void Consume(ID3D11Resource* resource, int32_t attribute_id) override;
public: ID3D11Resource* Expose(int32_t attribute_id) override;
private:
	Microsoft::WRL::ComPtr<ID3D11ComputeShader> _gravityCS;
	Microsoft::WRL::ComPtr<ID3D11ComputeShader> _buoyancyCS;

	int32_t _velocityInID{};
	int32_t _velocityOutID{};
};

