#pragma once

#include "DrawStage.h"

class DrawVolumeStage final : public DrawStage
{
public:  explicit DrawVolumeStage(ID3D11DeviceContext& context);
public:  virtual ~DrawVolumeStage() override = default;
public:  void Run(ID3D11DeviceContext& context) override;
public:  void Consume(ID3D11Resource* resource, int32_t attribute_id) override;
private: virtual void InitRS() override;
private: virtual void InitBS() override;
private:
	int32_t _volumeTexID{};
	int32_t _velocityID{};

	Microsoft::WRL::ComPtr<ID3D11RasterizerState>    _rsBack;
	Microsoft::WRL::ComPtr<ID3D11BlendState>         _bsAlpha;

	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> _frontFacesUvwSRV;
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView>   _frontFacesUvwRTV;

	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> _frontFacesWorldSRV;
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView>   _frontFacesWorldRTV;

	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> _backFacesUvwSRV;
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView>   _backFacesUvwRTV;

	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> _backFacesWorldSRV;
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView>   _backFacesWorldRTV;

	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> _jitterSRV;

	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>  _volumeTexView;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>  _velocityView;
};

