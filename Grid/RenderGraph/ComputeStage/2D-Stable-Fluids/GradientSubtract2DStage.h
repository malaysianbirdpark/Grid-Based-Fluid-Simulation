#pragma once

#include "Compute2DStage.h"

class GradientSubtract2DStage final : public Compute2DStage
{
public: GradientSubtract2DStage();
public: void Run(ID3D11DeviceContext& context) override;
public: ~GradientSubtract2DStage() override = default;
public: void Consume(ID3D11Resource* resource, int32_t attribute_id) override;
public: ID3D11Resource* Expose(int32_t attribute_id) override;
private:
	int32_t _xInID{};
	int32_t _subTargetID{};
	int32_t _resultID{};

	Microsoft::WRL::ComPtr<ID3D11ComputeShader>       _normalizer;
	Microsoft::WRL::ComPtr<ID3D11Texture2D>           _velocityUnorm;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>  _velocityUnormSR;
	Microsoft::WRL::ComPtr<ID3D11UnorderedAccessView> _velocityUnormView;
};

