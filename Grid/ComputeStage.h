#pragma once

#include <vector>

struct ComputeStage
{
public:  explicit ComputeStage(
	ID3D11Device& device,
	char const* compute_shader_path,
	std::vector<Microsoft::WRL::ComPtr<ID3D11UnorderedAccessView>>&& uav,
	std::vector<Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>>&& srv,
	int32_t group_x,
	int32_t group_y,
	int32_t group_z
);

public: int32_t                _parent{-1};
public: std::vector<int32_t>   _child{};
public: int32_t                _id{-1};

private: Microsoft::WRL::ComPtr<ID3D11ComputeShader>                     _cs;
private: std::vector<Microsoft::WRL::ComPtr<ID3D11UnorderedAccessView>>  _uav;
private: std::vector<Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>>   _srv;

private: std::vector<ID3D11ShaderResourceView*>                    _nullSrv{};
private: std::vector<ID3D11UnorderedAccessView*>                   _nullUav{};

private: int32_t _groupX{};
private: int32_t _groupY{};
private: int32_t _groupZ{};

public:  void Run(ID3D11DeviceContext& context);
private: void SetBarrier(ID3D11DeviceContext& context);
};

