#pragma once

#include <vector>

class CSTest
{
public:
    CSTest(ID3D11Device& device, IDXGISwapChain& swap_chain);
    void Run(ID3D11DeviceContext& context);
private: void SetBarrier(ID3D11DeviceContext& context);
private:
	Microsoft::WRL::ComPtr<ID3D11ComputeShader>                     _cs;
	Microsoft::WRL::ComPtr<ID3D11UnorderedAccessView>               _uav;
	IDXGISwapChain& swap_chain;
private:
	int _groupX;
	int _groupY;
	int _groupZ;
};

