#pragma once

#include "Compute3DStage.h"
#include "CBTimestep.h"

class MCAdvection3DStage final : public Compute3DStage
{
public: MCAdvection3DStage();
public: virtual ~MCAdvection3DStage() override = default;
public: void Run(ID3D11DeviceContext& context) override;
public: void Consume(ID3D11Resource* resource, int32_t attribute_id) override;
public: ID3D11Resource* Expose(int32_t attribute_id) override;
private:
	Microsoft::WRL::ComPtr<ID3D11ComputeShader> _firstAdvectionCS;
	Microsoft::WRL::ComPtr<ID3D11ComputeShader> _reverseAdvectionCS;
	Microsoft::WRL::ComPtr<ID3D11ComputeShader> _MCAdvectionCS;

	int32_t _velocityInID{};
	int32_t _velocityOutID{};
	int32_t _densityInID{};
	int32_t _densityOutID{};

	CBTimestep _ts{};
};

