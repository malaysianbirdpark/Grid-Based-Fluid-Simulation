#pragma once

#include "Compute2DStage.h"

class Advection2DStage final : public Compute2DStage
{
public: Advection2DStage();
public: virtual ~Advection2DStage() override = default;
public: void Run(ID3D11DeviceContext& context) override;
public: void Consume(ID3D11Resource* resource, int32_t attribute_id) override;
public: ID3D11Resource* Expose(int32_t attribute_id) override;
private:
	int32_t _cbTimestepID{};
	int32_t _velocityInID{};
	int32_t _velocityOutID{};
	int32_t _quantityInID{};
	int32_t _quantityOutID{};

	Microsoft::WRL::ComPtr<ID3D11ComputeShader>       _normalizer;
	Microsoft::WRL::ComPtr<ID3D11Texture2D>           _velocityUnorm;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>  _velocityUnormSR;
	Microsoft::WRL::ComPtr<ID3D11UnorderedAccessView> _velocityUnormView;
};

