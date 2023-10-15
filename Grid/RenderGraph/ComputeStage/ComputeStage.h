#pragma once

#include <vector>
#include <string>

struct ComputeStage
{
public:  explicit ComputeStage(
	char const* name,
	ID3D11Device& device,
	char const* compute_shader_path,
	UINT group_x,
	UINT group_y,
	UINT group_z
);

public: int32_t                     _id{-1};
public: std::string                 _name{};
public:  inline static std::string  _stageName{ "ComputeStage" };
public: std::vector<int32_t>        _incoming{};
public: std::vector<int32_t>        _outgoing{};

private: Microsoft::WRL::ComPtr<ID3D11ComputeShader>                     _cs;
private: std::vector<Microsoft::WRL::ComPtr<ID3D11UnorderedAccessView>>  _uav;
private: std::vector<Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>>   _srv;

private: std::vector<ID3D11ShaderResourceView*>                          _nullSrv{};
private: std::vector<ID3D11UnorderedAccessView*>                         _nullUav{};

private: UINT _groupX{};
private: UINT _groupY{};
private: UINT _groupZ{};

public:  void Run(ID3D11DeviceContext& context);
public:  virtual void RenderNode() const;
public:  virtual void Consume(ID3D11Resource* resource, int32_t attribute_id) = 0;
public:  virtual ID3D11Resource* Expose(int32_t attribute_id) = 0;
private: void SetBarrier(ID3D11DeviceContext& context);
};

