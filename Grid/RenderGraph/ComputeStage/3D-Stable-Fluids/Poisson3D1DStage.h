#pragma once

#include "Compute3DStage.h"

class Poisson3D1DStage final : public Compute3DStage
{
public: Poisson3D1DStage();
public: virtual ~Poisson3D1DStage() override = default;
public: void Run(ID3D11DeviceContext& context) override;
public: void Consume(ID3D11Resource* resource, int32_t attribute_id) override;
public: ID3D11Resource* Expose(int32_t attribute_id) override;
private:
	Microsoft::WRL::ComPtr<ID3D11ComputeShader> _initCS;
	Microsoft::WRL::ComPtr<ID3D11ComputeShader> _bcCS;

	int32_t _divID{};
	int32_t _pressureOutID{};
};

