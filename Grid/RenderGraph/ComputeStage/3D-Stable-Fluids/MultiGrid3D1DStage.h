#pragma once

#include "Compute3DStage.h"

class MultiGrid3D1DStage final : public Compute3DStage
{
	enum {
		Ah = 0,
		Ah_temp,
		Ah_residual,
		Ah_int,
		Ah_rhs,
		Dh,
		A2h,
		A2h_temp,
		A2h_residual,
		A2h_int,
		A2h_rhs,
		D2h,
		A4h,
		A4h_temp,
		A4h_residual,
		A4h_int,
		A4h_rhs,
		D4h,
		A8h,
		A8h_temp,
		A8h_residual,
		A8h_int,
		A8h_rhs,
		D8h,
		A16h,
		A16h_temp,
		A16h_residual,
		A16h_int,
		A16h_rhs,
		D16h,
		A32h,
		A32h_temp,
		A32h_residual,
		A32h_int,
		A32h_rhs,
		D32h,
		A64h,
		A64h_temp,
		A64h_residual,
		A64h_int,
		A64h_rhs,
		D64h,
		Final,
	};

public: MultiGrid3D1DStage();
public: virtual ~MultiGrid3D1DStage() override = default;
public: void Run(ID3D11DeviceContext& context) override;
public: void Consume(ID3D11Resource* resource, int32_t attribute_id) override;
public: ID3D11Resource* Expose(int32_t attribute_id) override;
private:
	Microsoft::WRL::ComPtr<ID3D11ComputeShader> _initCS;
	Microsoft::WRL::ComPtr<ID3D11ComputeShader> _mgJacobiCS;
	Microsoft::WRL::ComPtr<ID3D11ComputeShader> _residualCS;
	Microsoft::WRL::ComPtr<ID3D11ComputeShader> _subtractCS;
	Microsoft::WRL::ComPtr<ID3D11ComputeShader> _restrictCS;
	Microsoft::WRL::ComPtr<ID3D11ComputeShader> _interpolateCS;

	int32_t _divID{};
	int32_t _pressureOutID{};
};

