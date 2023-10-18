#pragma once

#include <vector>
#include <string>
#include <unordered_map>

struct ComputeStage
{
public:
    explicit ComputeStage(
	char const* name,
	char const* compute_shader_path,
	UINT group_x,
	UINT group_y,
	UINT group_z
);
virtual ~ComputeStage() = default;

public: int32_t                     _id{-1};
public: std::string                 _name{};
public:  inline static std::string  _stageName{ "ComputeStage" };
public: std::unordered_map<int32_t, int32_t> _incoming{};
public: std::unordered_map<int32_t, int32_t> _outgoing{};
public:  std::unordered_map<int32_t, std::string> _attrNames;

private:   Microsoft::WRL::ComPtr<ID3D11ComputeShader>                     _cs;
protected: std::vector<Microsoft::WRL::ComPtr<ID3D11Texture2D>>            _resource;
protected: std::vector<Microsoft::WRL::ComPtr<ID3D11UnorderedAccessView>>  _uav;
protected: std::vector<Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>>   _srv;

protected: std::vector<ID3D11ShaderResourceView*>                          _nullSrv{};
protected: std::vector<ID3D11UnorderedAccessView*>                         _nullUav{};

private: UINT _groupX{};
private: UINT _groupY{};
private: UINT _groupZ{};

public:  virtual void Run(ID3D11DeviceContext& context);
public:  void RenderNode() const;
public:  virtual void Consume(ID3D11Resource* resource, int32_t attribute_id) = 0;
public:  virtual ID3D11Resource* Expose(int32_t attribute_id) = 0;
private: void SetBarrier(ID3D11DeviceContext& context) const;
};

