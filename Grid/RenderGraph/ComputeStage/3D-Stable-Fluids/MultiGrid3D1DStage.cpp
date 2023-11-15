#include "pch.h"
#include "MultiGrid3D1DStage.h"

#include <d3dcompiler.h>

#include "imnodes.h"
#include "NodeManager.h"

MultiGrid3D1DStage::MultiGrid3D1DStage()
    : Compute3DStage{"3D1D-MultiGrid", "./CSO/MultiGrid3D1D_CS.cso", 8, 8, 8}
{
    _uav.resize(11);
    _srv.resize(12);
    _nullUav.resize(11);
    _nullSrv.resize(12);

    _resource.resize(11);

	auto desc {CD3D11_TEXTURE3D_DESC{}};
	desc.BindFlags = D3D11_BIND_UNORDERED_ACCESS | D3D11_BIND_SHADER_RESOURCE;
	desc.Usage = D3D11_USAGE_DEFAULT;
	desc.CPUAccessFlags = 0u;
	desc.Width = gSimulationInfo.width;
	desc.Height = gSimulationInfo.height;
	desc.Depth = gSimulationInfo.depth;
	desc.MipLevels = 1u;

    desc.Format = DXGI_FORMAT_R16_FLOAT;

	// Ah
    pDevice->CreateTexture3D(&desc, nullptr, _resource[0].ReleaseAndGetAddressOf());
    pDevice->CreateUnorderedAccessView(_resource[0].Get(), nullptr, _uav[0].ReleaseAndGetAddressOf());
    pDevice->CreateShaderResourceView(_resource[0].Get(), nullptr, _srv[0].ReleaseAndGetAddressOf());

	// Ah_temp
    pDevice->CreateTexture3D(&desc, nullptr, _resource[1].ReleaseAndGetAddressOf());
    pDevice->CreateUnorderedAccessView(_resource[1].Get(), nullptr, _uav[1].ReleaseAndGetAddressOf());
    pDevice->CreateShaderResourceView(_resource[1].Get(), nullptr, _srv[1].ReleaseAndGetAddressOf());

	// A2h
	desc.Width = gSimulationInfo.width >> 1;
	desc.Height = gSimulationInfo.height >> 1;
	desc.Depth = gSimulationInfo.depth >> 1;
    pDevice->CreateTexture3D(&desc, nullptr, _resource[2].ReleaseAndGetAddressOf());
    pDevice->CreateUnorderedAccessView(_resource[2].Get(), nullptr, _uav[2].ReleaseAndGetAddressOf());
    pDevice->CreateShaderResourceView(_resource[2].Get(), nullptr, _srv[2].ReleaseAndGetAddressOf());

	// A2h_temp
    pDevice->CreateTexture3D(&desc, nullptr, _resource[3].ReleaseAndGetAddressOf());
    pDevice->CreateUnorderedAccessView(_resource[3].Get(), nullptr, _uav[3].ReleaseAndGetAddressOf());
    pDevice->CreateShaderResourceView(_resource[3].Get(), nullptr, _srv[3].ReleaseAndGetAddressOf());

	// A4h
	desc.Width = gSimulationInfo.width >> 2;
	desc.Height = gSimulationInfo.height >> 2;
	desc.Depth = gSimulationInfo.depth >> 2;
    pDevice->CreateTexture3D(&desc, nullptr, _resource[4].ReleaseAndGetAddressOf());
    pDevice->CreateUnorderedAccessView(_resource[4].Get(), nullptr, _uav[4].ReleaseAndGetAddressOf());
    pDevice->CreateShaderResourceView(_resource[4].Get(), nullptr, _srv[4].ReleaseAndGetAddressOf());

	// A4h_temp
    pDevice->CreateTexture3D(&desc, nullptr, _resource[5].ReleaseAndGetAddressOf());
    pDevice->CreateUnorderedAccessView(_resource[5].Get(), nullptr, _uav[5].ReleaseAndGetAddressOf());
    pDevice->CreateShaderResourceView(_resource[5].Get(), nullptr, _srv[5].ReleaseAndGetAddressOf());

	// A8h
	desc.Width = gSimulationInfo.width >> 3;
	desc.Height = gSimulationInfo.height >> 3;
	desc.Depth = gSimulationInfo.depth >> 3;
    pDevice->CreateTexture3D(&desc, nullptr, _resource[6].ReleaseAndGetAddressOf());
    pDevice->CreateUnorderedAccessView(_resource[6].Get(), nullptr, _uav[6].ReleaseAndGetAddressOf());
    pDevice->CreateShaderResourceView(_resource[6].Get(), nullptr, _srv[6].ReleaseAndGetAddressOf());

	// A8h_temp
    pDevice->CreateTexture3D(&desc, nullptr, _resource[7].ReleaseAndGetAddressOf());
    pDevice->CreateUnorderedAccessView(_resource[7].Get(), nullptr, _uav[7].ReleaseAndGetAddressOf());
    pDevice->CreateShaderResourceView(_resource[7].Get(), nullptr, _srv[7].ReleaseAndGetAddressOf());

	// A16h
	desc.Width = gSimulationInfo.width >> 4;
	desc.Height = gSimulationInfo.height >> 4;
	desc.Depth = gSimulationInfo.depth >> 4;
    pDevice->CreateTexture3D(&desc, nullptr, _resource[8].ReleaseAndGetAddressOf());
    pDevice->CreateUnorderedAccessView(_resource[8].Get(), nullptr, _uav[8].ReleaseAndGetAddressOf());
    pDevice->CreateShaderResourceView(_resource[8].Get(), nullptr, _srv[8].ReleaseAndGetAddressOf());

	// A16h_temp
    pDevice->CreateTexture3D(&desc, nullptr, _resource[9].ReleaseAndGetAddressOf());
    pDevice->CreateUnorderedAccessView(_resource[9].Get(), nullptr, _uav[9].ReleaseAndGetAddressOf());
    pDevice->CreateShaderResourceView(_resource[9].Get(), nullptr, _srv[9].ReleaseAndGetAddressOf());

	// Final Result
	desc.Width = gSimulationInfo.width;
	desc.Height = gSimulationInfo.height;
	desc.Depth = gSimulationInfo.depth;
    pDevice->CreateTexture3D(&desc, nullptr, _resource[10].ReleaseAndGetAddressOf());
    pDevice->CreateUnorderedAccessView(_resource[10].Get(), nullptr, _uav[10].ReleaseAndGetAddressOf());
    pDevice->CreateShaderResourceView(_resource[10].Get(), nullptr, _srv[10].ReleaseAndGetAddressOf());

    _divID = NodeManager::IssueIncomingAttrID();
    _incoming[_divID] = -1;
    _attrNames[_divID] = { "Div" };

    _pressureOutID = NodeManager::IssueOutgoingAttrID();
    _outgoing[_pressureOutID] = -1;
    _attrNames[_pressureOutID] = { "Pressure" };

    {
		std::string _path{"./CSO/Initializer3D_CS.cso"};
		std::wstring p(_path.length(), L' ');
		std::ranges::copy(_path, p.begin());

		Microsoft::WRL::ComPtr<ID3DBlob> pBlob;
		D3DReadFileToBlob(
			p.c_str(),
			pBlob.ReleaseAndGetAddressOf()
		);

		pDevice->CreateComputeShader(
			pBlob->GetBufferPointer(), 
			pBlob->GetBufferSize(),
			nullptr,
			_initCS.ReleaseAndGetAddressOf()
		);
    }

    {
		std::string _path{"./CSO/Restrict3D_CS.cso"};
		std::wstring p(_path.length(), L' ');
		std::ranges::copy(_path, p.begin());

		Microsoft::WRL::ComPtr<ID3DBlob> pBlob;
		D3DReadFileToBlob(
			p.c_str(),
			pBlob.ReleaseAndGetAddressOf()
		);

		pDevice->CreateComputeShader(
			pBlob->GetBufferPointer(), 
			pBlob->GetBufferSize(),
			nullptr,
			_restrictCS.ReleaseAndGetAddressOf()
		);
    }

    {
		std::string _path{"./CSO/Interpolate3D_CS.cso"};
		std::wstring p(_path.length(), L' ');
		std::ranges::copy(_path, p.begin());

		Microsoft::WRL::ComPtr<ID3DBlob> pBlob;
		D3DReadFileToBlob(
			p.c_str(),
			pBlob.ReleaseAndGetAddressOf()
		);

		pDevice->CreateComputeShader(
			pBlob->GetBufferPointer(), 
			pBlob->GetBufferSize(),
			nullptr,
			_interpolateCS.ReleaseAndGetAddressOf()
		);
    }
}

void MultiGrid3D1DStage::Run(ID3D11DeviceContext& context)
{
    context.OMSetRenderTargets(0u, nullptr, nullptr);

	context.CSSetUnorderedAccessViews(0u, 2u, _uav[0].GetAddressOf(), nullptr);
	context.CSSetShader(_initCS.Get(), nullptr, 0u);
	context.Dispatch(
		static_cast<UINT>(ceil(static_cast<float>(gSimulationInfo.width) / _groupX)),
		static_cast<UINT>(ceil(static_cast<float>(gSimulationInfo.height) / _groupY)), 
		static_cast<UINT>(ceil(static_cast<float>(gSimulationInfo.depth) / _groupZ)) 
	);
	SetBarrier(context);

	// Jacobi for Ah
	context.CSSetShaderResources(0u, 1u, _srv[0].GetAddressOf());
	context.CSSetShaderResources(1u, 1u, _srv[11].GetAddressOf());
	context.CSSetUnorderedAccessViews(0u, 1u, _uav[1].GetAddressOf(), nullptr);
	context.CSSetShader(_cs.Get(), nullptr, 0u);
	context.Dispatch(
		static_cast<UINT>(ceil(static_cast<float>(gSimulationInfo.width) / _groupX)),
		static_cast<UINT>(ceil(static_cast<float>(gSimulationInfo.height) / _groupY)), 
		static_cast<UINT>(ceil(static_cast<float>(gSimulationInfo.depth) / _groupZ)) 
	);
	SetBarrier(context);

	// Restrict Ah to A2h
	context.CSSetShaderResources(0u, 1u, _srv[1].GetAddressOf());
	context.CSSetUnorderedAccessViews(0u, 1u, _uav[2].GetAddressOf(), nullptr);
	context.CSSetShader(_restrictCS.Get(), nullptr, 0u);
	context.Dispatch(
		static_cast<UINT>(ceil(static_cast<float>(gSimulationInfo.width) / _groupX)),
		static_cast<UINT>(ceil(static_cast<float>(gSimulationInfo.height) / _groupY)), 
		static_cast<UINT>(ceil(static_cast<float>(gSimulationInfo.depth) / _groupZ)) 
	);
	SetBarrier(context);

	// Jacobi for A2h
	context.CSSetShaderResources(0u, 1u, _srv[2].GetAddressOf());
	context.CSSetShaderResources(1u, 1u, _srv[11].GetAddressOf());
	context.CSSetUnorderedAccessViews(0u, 1u, _uav[3].GetAddressOf(), nullptr);
	context.CSSetShader(_cs.Get(), nullptr, 0u);
	context.Dispatch(
		static_cast<UINT>(ceil(static_cast<float>(gSimulationInfo.width) / _groupX)),
		static_cast<UINT>(ceil(static_cast<float>(gSimulationInfo.height) / _groupY)), 
		static_cast<UINT>(ceil(static_cast<float>(gSimulationInfo.depth) / _groupZ)) 
	);
	SetBarrier(context);

	// Restrict A2h to A4h
	context.CSSetShaderResources(0u, 1u, _srv[3].GetAddressOf());
	context.CSSetUnorderedAccessViews(0u, 1u, _uav[4].GetAddressOf(), nullptr);
	context.CSSetShader(_restrictCS.Get(), nullptr, 0u);
	context.Dispatch(
		static_cast<UINT>(ceil(static_cast<float>(gSimulationInfo.width) / _groupX)),
		static_cast<UINT>(ceil(static_cast<float>(gSimulationInfo.height) / _groupY)), 
		static_cast<UINT>(ceil(static_cast<float>(gSimulationInfo.depth) / _groupZ)) 
	);
	SetBarrier(context);

	// Jacobi for A4h
	context.CSSetShaderResources(0u, 1u, _srv[4].GetAddressOf());
	context.CSSetShaderResources(1u, 1u, _srv[11].GetAddressOf());
	context.CSSetUnorderedAccessViews(0u, 1u, _uav[5].GetAddressOf(), nullptr);
	context.CSSetShader(_cs.Get(), nullptr, 0u);
	context.Dispatch(
		static_cast<UINT>(ceil(static_cast<float>(gSimulationInfo.width) / _groupX)),
		static_cast<UINT>(ceil(static_cast<float>(gSimulationInfo.height) / _groupY)), 
		static_cast<UINT>(ceil(static_cast<float>(gSimulationInfo.depth) / _groupZ)) 
	);
	SetBarrier(context);

	// Restrict A4h to A8h
	context.CSSetShaderResources(0u, 1u, _srv[5].GetAddressOf());
	context.CSSetUnorderedAccessViews(0u, 1u, _uav[6].GetAddressOf(), nullptr);
	context.CSSetShader(_restrictCS.Get(), nullptr, 0u);
	context.Dispatch(
		static_cast<UINT>(ceil(static_cast<float>(gSimulationInfo.width) / _groupX)),
		static_cast<UINT>(ceil(static_cast<float>(gSimulationInfo.height) / _groupY)), 
		static_cast<UINT>(ceil(static_cast<float>(gSimulationInfo.depth) / _groupZ)) 
	);
	SetBarrier(context);

	// Jacobi for A8h
	context.CSSetShaderResources(0u, 1u, _srv[6].GetAddressOf());
	context.CSSetShaderResources(1u, 1u, _srv[11].GetAddressOf());
	context.CSSetUnorderedAccessViews(0u, 1u, _uav[7].GetAddressOf(), nullptr);
	context.CSSetShader(_cs.Get(), nullptr, 0u);
	context.Dispatch(
		static_cast<UINT>(ceil(static_cast<float>(gSimulationInfo.width) / _groupX)),
		static_cast<UINT>(ceil(static_cast<float>(gSimulationInfo.height) / _groupY)), 
		static_cast<UINT>(ceil(static_cast<float>(gSimulationInfo.depth) / _groupZ)) 
	);
	SetBarrier(context);

	// Restrict A8h to A16h
	context.CSSetShaderResources(0u, 1u, _srv[7].GetAddressOf());
	context.CSSetUnorderedAccessViews(0u, 1u, _uav[8].GetAddressOf(), nullptr);
	context.CSSetShader(_restrictCS.Get(), nullptr, 0u);
	context.Dispatch(
		static_cast<UINT>(ceil(static_cast<float>(gSimulationInfo.width) / _groupX)),
		static_cast<UINT>(ceil(static_cast<float>(gSimulationInfo.height) / _groupY)), 
		static_cast<UINT>(ceil(static_cast<float>(gSimulationInfo.depth) / _groupZ)) 
	);
	SetBarrier(context);

	// Jacobi for A16h
	context.CSSetShaderResources(0u, 1u, _srv[8].GetAddressOf());
	context.CSSetShaderResources(1u, 1u, _srv[11].GetAddressOf());
	context.CSSetUnorderedAccessViews(0u, 1u, _uav[9].GetAddressOf(), nullptr);
	context.CSSetShader(_cs.Get(), nullptr, 0u);
	context.Dispatch(
		static_cast<UINT>(ceil(static_cast<float>(gSimulationInfo.width) / _groupX)),
		static_cast<UINT>(ceil(static_cast<float>(gSimulationInfo.height) / _groupY)), 
		static_cast<UINT>(ceil(static_cast<float>(gSimulationInfo.depth) / _groupZ)) 
	);
	SetBarrier(context);
}

void MultiGrid3D1DStage::Consume(ID3D11Resource* resource, int32_t attribute_id)
{
    if (attribute_id == _divID) {
        pDevice->CreateShaderResourceView(resource, nullptr, _srv[11].ReleaseAndGetAddressOf());
    }
}

ID3D11Resource* MultiGrid3D1DStage::Expose(int32_t attribute_id)
{
    if (attribute_id == _pressureOutID)
        return static_cast<ID3D11Resource*>(_resource[10].Get());
	return nullptr;
}

