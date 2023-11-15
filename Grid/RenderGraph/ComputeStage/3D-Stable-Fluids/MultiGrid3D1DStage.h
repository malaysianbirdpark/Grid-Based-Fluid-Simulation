#pragma once

#include "Compute3DStage.h"

class MultiGrid3D1DStage final : public Compute3DStage
{
	enum {
		Ah = 0,
		Ah_residual,
		A2h,
		A2h_residual,
		A2h_rhs,
		A4h,
		A4h_residual,
		A4h_rhs,
		A8h,
		A8h_residual,
		A8h_rhs,
		A16h,
		A16h_residual,
		A16h_rhs,
		A32h,
		A32h_residual,
		A32h_rhs,
		A64h,
		A64h_residual,
		A64h_rhs,
		Final,
		Div
	};

public: MultiGrid3D1DStage();
public: virtual ~MultiGrid3D1DStage() override = default;
public: void Run(ID3D11DeviceContext& context) override;
public: void Consume(ID3D11Resource* resource, int32_t attribute_id) override;
public: ID3D11Resource* Expose(int32_t attribute_id) override;
private:
	Microsoft::WRL::ComPtr<ID3D11ComputeShader> _initCS;
	Microsoft::WRL::ComPtr<ID3D11ComputeShader> _restrictCS;
	Microsoft::WRL::ComPtr<ID3D11ComputeShader> _interpolateCS;

	int32_t _divID{};
	int32_t _pressureOutID{};
};

