#pragma once

#include "Compute2DStage.h"

class Poisson2D1DStage final : public Compute2DStage
{
public: Poisson2D1DStage();
public: virtual ~Poisson2D1DStage() override = default;
public: void Run(ID3D11DeviceContext& context) override;
public: void Consume(ID3D11Resource* resource, int32_t attribute_id) override;
public: ID3D11Resource* Expose(int32_t attribute_id) override;
private:
	int32_t _xInID{};
	int32_t _bID{};
	int32_t _constID{};
	int32_t _xOutID{};

	Microsoft::WRL::ComPtr<ID3D11ComputeShader>       _normalizer;
	Microsoft::WRL::ComPtr<ID3D11Texture2D>           _resultUnorm;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>  _resultUnormSR;
	Microsoft::WRL::ComPtr<ID3D11UnorderedAccessView> _resultUnormView;
};

