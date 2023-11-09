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
	Microsoft::WRL::ComPtr<ID3D11DepthStencilState>              _readDS;

    Microsoft::WRL::ComPtr<ID3D11Texture2D>                      _voxelDSB;
	std::vector<Microsoft::WRL::ComPtr<ID3D11DepthStencilView>>  _voxelDSV;
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>             _dsvSRV;

    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>             _voxelSRV;
    Microsoft::WRL::ComPtr<ID3D11UnorderedAccessView>            _voxelUAV;

	std::vector<Microsoft::WRL::ComPtr<ID3D11RenderTargetView>>  _velocityRTV;
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>             _velocitySRV;
    Microsoft::WRL::ComPtr<ID3D11UnorderedAccessView>            _velocityUAV;

    Microsoft::WRL::ComPtr<ID3D11ComputeShader>                  _boundingBoxCS;
private:
	std::unique_ptr<class PipelineStateObject>                   _obstaclePSO;
	std::unique_ptr<class PipelineStateObject>                   _vertexVelocityPSO;

	std::shared_ptr<class DrawSceneStage>                        _targetScene;
	std::shared_ptr<class DrawVolumeStage>                       _targetVolume;

	std::unique_ptr<class InverseTransform>                      _inverseTransform;

	std::unique_ptr<class CBVoxelization>                        _cb;
};

