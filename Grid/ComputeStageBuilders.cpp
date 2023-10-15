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

std::shared_ptr<ComputeStage> StableFluid2D_Sourcing(ID3D11Device& device) {
	std::vector<Microsoft::WRL::ComPtr<ID3D11UnorderedAccessView>> uav{};
	std::vector<Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>> srv{};

	uav.resize(2);
	srv.resize(1);

	// velocity
	{
        Microsoft::WRL::ComPtr<ID3D11Texture2D> velocity_buf {};
        auto desc {CD3D11_TEXTURE2D_DESC{}};
		desc.BindFlags = D3D11_BIND_UNORDERED_ACCESS;
		desc.Usage = D3D11_USAGE_DEFAULT;
		desc.CPUAccessFlags = 0u;
		desc.Format = DXGI_FORMAT_R16G16_FLOAT;
		desc.Width = 800;
		desc.Height = 600;
		desc.MipLevels = 1u;
		desc.ArraySize = 1u;
		desc.SampleDesc.Count = 1;

		auto uav_desc {CD3D11_UNORDERED_ACCESS_VIEW_DESC{}};
		uav_desc.Format = DXGI_FORMAT_R16G16_FLOAT;
		uav_desc.ViewDimension = D3D11_UAV_DIMENSION_TEXTURE2D;
		device.CreateTexture2D(&desc, nullptr, velocity_buf.ReleaseAndGetAddressOf());
		device.CreateUnorderedAccessView(velocity_buf.Get(), nullptr, uav[0].ReleaseAndGetAddressOf());
	}

	// density
	{
        Microsoft::WRL::ComPtr<ID3D11Texture2D> density_buf {};
        auto desc {CD3D11_TEXTURE2D_DESC{}};
		desc.BindFlags = D3D11_BIND_UNORDERED_ACCESS;
		desc.Usage = D3D11_USAGE_DEFAULT;
		desc.CPUAccessFlags = 0u;
		desc.Format = DXGI_FORMAT_R16G16B16A16_FLOAT;
		desc.Width = 800;
		desc.Height = 600;
		desc.MipLevels = 1u;
		desc.ArraySize = 1u;
		desc.SampleDesc.Count = 1;

		auto uav_desc {CD3D11_UNORDERED_ACCESS_VIEW_DESC{}};
		uav_desc.Format = DXGI_FORMAT_R16G16B16A16_FLOAT;
		uav_desc.ViewDimension = D3D11_UAV_DIMENSION_TEXTURE2D;
		device.CreateTexture2D(&desc, nullptr, density_buf.ReleaseAndGetAddressOf());
		device.CreateUnorderedAccessView(density_buf.Get(), nullptr, uav[1].ReleaseAndGetAddressOf());
	}

	return std::move(std::make_shared<ComputeStage>("fluid sourcing test", device, "SourcingTestCS.cso", std::move(uav), std::move(srv), static_cast<int32_t>(ceil(800.0f / 32)), static_cast<UINT>(ceil(600.0f / 32)), 1));
}
