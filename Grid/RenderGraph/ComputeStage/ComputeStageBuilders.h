#include "ComputeStage.h"

#include <memory>

std::shared_ptr<ComputeStage> TestCS(ID3D11Device& device, IDXGISwapChain& swap_chain);
std::shared_ptr<ComputeStage> TestCS1(ID3D11Device& device, IDXGISwapChain& swap_chain);
std::shared_ptr<ComputeStage> TestCS2(ID3D11Device& device, IDXGISwapChain& swap_chain);

std::shared_ptr<ComputeStage> StableFluid2D_Sourcing(ID3D11Device& device);

