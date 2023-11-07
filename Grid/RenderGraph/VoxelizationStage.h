#pragma once

#include <vector>
#include <memory>

class VoxelizationStage
{
public: VoxelizationStage(ID3D11DeviceContext& context);
public: void Run(ID3D11DeviceContext& context);
public: void AddTargetScene(std::shared_ptr<class DrawSceneStage> target);
public: void AddTargetVolume(std::shared_ptr<class DrawVolumeStage> target);
private:
	D3D11_VIEWPORT                                               _vp{};

	Microsoft::WRL::ComPtr<ID3D11RasterizerState>                _rs;

	Microsoft::WRL::ComPtr<ID3D11DepthStencilState>              _voxelDS;
    Microsoft::WRL::ComPtr<ID3D11Texture2D>                      _voxelDSB;
	//std::vector<Microsoft::WRL::ComPtr<ID3D11DepthStencilView>>  _voxelDSV;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilView>               _voxelDSV;


	std::vector<Microsoft::WRL::ComPtr<ID3D11RenderTargetView>>  _voxelRTV;
    Microsoft::WRL::ComPtr<ID3D11UnorderedAccessView>            _voxelUAV;
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>             _voxelSRV;


	std::unique_ptr<class PipelineStateObject>                   _pso;

	std::shared_ptr<class DrawSceneStage>                        _targetScene;
	std::shared_ptr<class DrawVolumeStage>                       _targetVolume;

	std::unique_ptr<class InverseTransform>                      _inverseTransform;
};

