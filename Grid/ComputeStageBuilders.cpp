#include "pch.h"
#include "ComputeStageBuilders.h"

#include <vector>

std::shared_ptr<ComputeStage> TestCS(ID3D11Device& device, IDXGISwapChain& swap_chain) {
	std::vector<Microsoft::WRL::ComPtr<ID3D11UnorderedAccessView>> uav{};
	std::vector<Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>> srv{};

	uav.emplace_back(nullptr);
	srv.emplace_back(nullptr);

	Microsoft::WRL::ComPtr<ID3D11Resource> backbuffer;
	swap_chain.GetBuffer(0u, IID_PPV_ARGS(backbuffer.ReleaseAndGetAddressOf()));

	auto desc{ CD3D11_UNORDERED_ACCESS_VIEW_DESC{} };
	desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	desc.ViewDimension = D3D11_UAV_DIMENSION_TEXTURE2D;
	device.CreateUnorderedAccessView(backbuffer.Get(), &desc, uav.back().ReleaseAndGetAddressOf());

	return std::move(std::make_shared<ComputeStage>("cs-test0", device, "testCS.cso", std::move(uav), std::move(srv), static_cast<int32_t>(ceil(800.0f / 256)), 600, 1));
}

std::shared_ptr<ComputeStage> TestCS1(ID3D11Device& device, IDXGISwapChain& swap_chain)
{
	std::vector<Microsoft::WRL::ComPtr<ID3D11UnorderedAccessView>> uav{};
	std::vector<Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>> srv{};

	uav.emplace_back(nullptr);
	srv.emplace_back(nullptr);

	Microsoft::WRL::ComPtr<ID3D11Resource> backbuffer;
	swap_chain.GetBuffer(0u, IID_PPV_ARGS(backbuffer.ReleaseAndGetAddressOf()));

	auto desc{ CD3D11_UNORDERED_ACCESS_VIEW_DESC{} };
	desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	desc.ViewDimension = D3D11_UAV_DIMENSION_TEXTURE2D;
	device.CreateUnorderedAccessView(backbuffer.Get(), &desc, uav.back().ReleaseAndGetAddressOf());

	return std::move(std::make_shared<ComputeStage>("cs-test1", device, "test1CS.cso", std::move(uav), std::move(srv), static_cast<int32_t>(ceil(800.0f / 256)), 600, 1));
}

std::shared_ptr<ComputeStage> TestCS2(ID3D11Device& device, IDXGISwapChain& swap_chain)
{
	std::vector<Microsoft::WRL::ComPtr<ID3D11UnorderedAccessView>> uav{};
	std::vector<Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>> srv{};

	uav.emplace_back(nullptr);
	srv.emplace_back(nullptr);

	Microsoft::WRL::ComPtr<ID3D11Resource> backbuffer;
	swap_chain.GetBuffer(0u, IID_PPV_ARGS(backbuffer.ReleaseAndGetAddressOf()));

	auto desc{ CD3D11_UNORDERED_ACCESS_VIEW_DESC{} };
	desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	desc.ViewDimension = D3D11_UAV_DIMENSION_TEXTURE2D;
	device.CreateUnorderedAccessView(backbuffer.Get(), &desc, uav.back().ReleaseAndGetAddressOf());

	return std::move(std::make_shared<ComputeStage>("cs-test2", device, "test2CS.cso", std::move(uav), std::move(srv), static_cast<int32_t>(ceil(800.0f / 256)), 600, 1));
}
