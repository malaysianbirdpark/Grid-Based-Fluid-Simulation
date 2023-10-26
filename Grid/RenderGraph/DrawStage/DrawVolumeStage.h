#pragma once

#include "DrawStage.h"

class DrawVolumeStage final : public DrawStage
{
public:  explicit DrawVolumeStage(ID3D11DeviceContext& context);
public:  virtual ~DrawVolumeStage() override = default;
public:  void Run(ID3D11DeviceContext& context) override;
public:  void Consume(ID3D11Resource* resource, int32_t attribute_id) override;
private:
	int32_t _volumeTexID{};

	Microsoft::WRL::ComPtr<ID3D11RasterizerState>    _rsBack;

	Microsoft::WRL::ComPtr<ID3D11Texture2D>          _frontFacesBuffer;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> _frontFacesSRV;
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView>   _frontFacesRTV;

	Microsoft::WRL::ComPtr<ID3D11Texture2D>          _backFacesBuffer;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> _backFacesSRV;
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView>   _backFacesRTV;

	Microsoft::WRL::ComPtr<ID3D11Texture2D>          _frontFacesUvwBuffer;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> _frontFacesUvwSRV;
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView>   _frontFacesUvwRTV;

	Microsoft::WRL::ComPtr<ID3D11Texture2D>          _backFacesUvwBuffer;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> _backFacesUvwSRV;
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView>   _backFacesUvwRTV;

	Microsoft::WRL::ComPtr<ID3D11Texture3D>          _uvwBuffer;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> _uvwSRV;
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView>   _uvwRTV;

	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>  _volumeTexView;
};

