#pragma once

#include <vector>

class ComputePipeline {
public:  explicit ComputePipeline(ID3D11Device& device, char const* path, int group_x, int group_y, int group_z);
public:  void Run(ID3D11DeviceContext& context);
private: void SetBarrier(ID3D11DeviceContext& context);
private:
	Microsoft::WRL::ComPtr<ID3D11ComputeShader>                     _cs;
	std::vector<Microsoft::WRL::ComPtr<ID3D11UnorderedAccessView>>  _uav;
private:
	int _groupX;
	int _groupY;
	int _groupZ;
};

