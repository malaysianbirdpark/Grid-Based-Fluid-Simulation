#include "pch.h"
#include "MultiGrid3D1DStage.h"

#include <d3dcompiler.h>

#include "imnodes.h"
#include "NodeManager.h"

MultiGrid3D1DStage::MultiGrid3D1DStage()
    : Compute3DStage{"3D1D-MultiGrid", "./CSO/Poisson3D1D_CS.cso", 8, 8, 8}
{
    _uav.resize(Final + 1);
    _srv.resize(Final + 2);
    _nullUav.resize(Final + 1);
    _nullSrv.resize(Final + 2);

    _resource.resize(Final + 1);

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
    pDevice->CreateTexture3D(&desc, nullptr, _resource[Ah].ReleaseAndGetAddressOf());
    pDevice->CreateUnorderedAccessView(_resource[Ah].Get(), nullptr, _uav[Ah].ReleaseAndGetAddressOf());
    pDevice->CreateShaderResourceView(_resource[Ah].Get(), nullptr, _srv[Ah].ReleaseAndGetAddressOf());

	// Ah_temp
    pDevice->CreateTexture3D(&desc, nullptr, _resource[Ah_temp].ReleaseAndGetAddressOf());
    pDevice->CreateUnorderedAccessView(_resource[Ah_temp].Get(), nullptr, _uav[Ah_temp].ReleaseAndGetAddressOf());
    pDevice->CreateShaderResourceView(_resource[Ah_temp].Get(), nullptr, _srv[Ah_temp].ReleaseAndGetAddressOf());

	// Ah_residual
    pDevice->CreateTexture3D(&desc, nullptr, _resource[Ah_residual].ReleaseAndGetAddressOf());
    pDevice->CreateUnorderedAccessView(_resource[Ah_residual].Get(), nullptr, _uav[Ah_residual].ReleaseAndGetAddressOf());
    pDevice->CreateShaderResourceView(_resource[Ah_residual].Get(), nullptr, _srv[Ah_residual].ReleaseAndGetAddressOf());

	// Ah_int
    pDevice->CreateTexture3D(&desc, nullptr, _resource[Ah_int].ReleaseAndGetAddressOf());
    pDevice->CreateUnorderedAccessView(_resource[Ah_int].Get(), nullptr, _uav[Ah_int].ReleaseAndGetAddressOf());
    pDevice->CreateShaderResourceView(_resource[Ah_int].Get(), nullptr, _srv[Ah_int].ReleaseAndGetAddressOf());

	// Ah_rhs
    pDevice->CreateTexture3D(&desc, nullptr, _resource[Ah_rhs].ReleaseAndGetAddressOf());
    pDevice->CreateUnorderedAccessView(_resource[Ah_rhs].Get(), nullptr, _uav[Ah_rhs].ReleaseAndGetAddressOf());
    pDevice->CreateShaderResourceView(_resource[Ah_rhs].Get(), nullptr, _srv[Ah_rhs].ReleaseAndGetAddressOf());

	// Dh
    pDevice->CreateTexture3D(&desc, nullptr, _resource[Dh].ReleaseAndGetAddressOf());
    pDevice->CreateUnorderedAccessView(_resource[Dh].Get(), nullptr, _uav[Dh].ReleaseAndGetAddressOf());
    pDevice->CreateShaderResourceView(_resource[Dh].Get(), nullptr, _srv[Dh].ReleaseAndGetAddressOf());

	// A2h
	desc.Width = gSimulationInfo.width >> 1;
	desc.Height = gSimulationInfo.height >> 1;
	desc.Depth = gSimulationInfo.depth >> 1;
    pDevice->CreateTexture3D(&desc, nullptr, _resource[A2h].ReleaseAndGetAddressOf());
    pDevice->CreateUnorderedAccessView(_resource[A2h].Get(), nullptr, _uav[A2h].ReleaseAndGetAddressOf());
    pDevice->CreateShaderResourceView(_resource[A2h].Get(), nullptr, _srv[A2h].ReleaseAndGetAddressOf());

	// A2h_temp
    pDevice->CreateTexture3D(&desc, nullptr, _resource[A2h_temp].ReleaseAndGetAddressOf());
    pDevice->CreateUnorderedAccessView(_resource[A2h_temp].Get(), nullptr, _uav[A2h_temp].ReleaseAndGetAddressOf());
    pDevice->CreateShaderResourceView(_resource[A2h_temp].Get(), nullptr, _srv[A2h_temp].ReleaseAndGetAddressOf());

	// A2h_residual
    pDevice->CreateTexture3D(&desc, nullptr, _resource[A2h_residual].ReleaseAndGetAddressOf());
    pDevice->CreateUnorderedAccessView(_resource[A2h_residual].Get(), nullptr, _uav[A2h_residual].ReleaseAndGetAddressOf());
    pDevice->CreateShaderResourceView(_resource[A2h_residual].Get(), nullptr, _srv[A2h_residual].ReleaseAndGetAddressOf());

	// A2h_rhs
    pDevice->CreateTexture3D(&desc, nullptr, _resource[A2h_rhs].ReleaseAndGetAddressOf());
    pDevice->CreateUnorderedAccessView(_resource[A2h_rhs].Get(), nullptr, _uav[A2h_rhs].ReleaseAndGetAddressOf());
    pDevice->CreateShaderResourceView(_resource[A2h_rhs].Get(), nullptr, _srv[A2h_rhs].ReleaseAndGetAddressOf());

	// D2h
    pDevice->CreateTexture3D(&desc, nullptr, _resource[D2h].ReleaseAndGetAddressOf());
    pDevice->CreateUnorderedAccessView(_resource[D2h].Get(), nullptr, _uav[D2h].ReleaseAndGetAddressOf());
    pDevice->CreateShaderResourceView(_resource[D2h].Get(), nullptr, _srv[D2h].ReleaseAndGetAddressOf());

	// A4h
	desc.Width = gSimulationInfo.width >> 2;
	desc.Height = gSimulationInfo.height >> 2;
	desc.Depth = gSimulationInfo.depth >> 2;
    pDevice->CreateTexture3D(&desc, nullptr, _resource[A4h].ReleaseAndGetAddressOf());
    pDevice->CreateUnorderedAccessView(_resource[A4h].Get(), nullptr, _uav[A4h].ReleaseAndGetAddressOf());
    pDevice->CreateShaderResourceView(_resource[A4h].Get(), nullptr, _srv[A4h].ReleaseAndGetAddressOf());

	// A4h_temp
    pDevice->CreateTexture3D(&desc, nullptr, _resource[A4h_temp].ReleaseAndGetAddressOf());
    pDevice->CreateUnorderedAccessView(_resource[A4h_temp].Get(), nullptr, _uav[A4h_temp].ReleaseAndGetAddressOf());
    pDevice->CreateShaderResourceView(_resource[A4h_temp].Get(), nullptr, _srv[A4h_temp].ReleaseAndGetAddressOf());

	// A4h_residual
    pDevice->CreateTexture3D(&desc, nullptr, _resource[A4h_residual].ReleaseAndGetAddressOf());
    pDevice->CreateUnorderedAccessView(_resource[A4h_residual].Get(), nullptr, _uav[A4h_residual].ReleaseAndGetAddressOf());
    pDevice->CreateShaderResourceView(_resource[A4h_residual].Get(), nullptr, _srv[A4h_residual].ReleaseAndGetAddressOf());

	// A4h_rhs
    pDevice->CreateTexture3D(&desc, nullptr, _resource[A4h_rhs].ReleaseAndGetAddressOf());
    pDevice->CreateUnorderedAccessView(_resource[A4h_rhs].Get(), nullptr, _uav[A4h_rhs].ReleaseAndGetAddressOf());
    pDevice->CreateShaderResourceView(_resource[A4h_rhs].Get(), nullptr, _srv[A4h_rhs].ReleaseAndGetAddressOf());

	// D4h
    pDevice->CreateTexture3D(&desc, nullptr, _resource[D4h].ReleaseAndGetAddressOf());
    pDevice->CreateUnorderedAccessView(_resource[D4h].Get(), nullptr, _uav[D4h].ReleaseAndGetAddressOf());
    pDevice->CreateShaderResourceView(_resource[D4h].Get(), nullptr, _srv[D4h].ReleaseAndGetAddressOf());

	// A8h
	desc.Width = gSimulationInfo.width >> 3;
	desc.Height = gSimulationInfo.height >> 3;
	desc.Depth = gSimulationInfo.depth >> 3;
    pDevice->CreateTexture3D(&desc, nullptr, _resource[A8h].ReleaseAndGetAddressOf());
    pDevice->CreateUnorderedAccessView(_resource[A8h].Get(), nullptr, _uav[A8h].ReleaseAndGetAddressOf());
    pDevice->CreateShaderResourceView(_resource[A8h].Get(), nullptr, _srv[A8h].ReleaseAndGetAddressOf());

	// A8h_temp
    pDevice->CreateTexture3D(&desc, nullptr, _resource[A8h_temp].ReleaseAndGetAddressOf());
    pDevice->CreateUnorderedAccessView(_resource[A8h_temp].Get(), nullptr, _uav[A8h_temp].ReleaseAndGetAddressOf());
    pDevice->CreateShaderResourceView(_resource[A8h_temp].Get(), nullptr, _srv[A8h_temp].ReleaseAndGetAddressOf());

	// A8h_residual
    pDevice->CreateTexture3D(&desc, nullptr, _resource[A8h_residual].ReleaseAndGetAddressOf());
    pDevice->CreateUnorderedAccessView(_resource[A8h_residual].Get(), nullptr, _uav[A8h_residual].ReleaseAndGetAddressOf());
    pDevice->CreateShaderResourceView(_resource[A8h_residual].Get(), nullptr, _srv[A8h_residual].ReleaseAndGetAddressOf());

	// A8h_rhs
    pDevice->CreateTexture3D(&desc, nullptr, _resource[A8h_rhs].ReleaseAndGetAddressOf());
    pDevice->CreateUnorderedAccessView(_resource[A8h_rhs].Get(), nullptr, _uav[A8h_rhs].ReleaseAndGetAddressOf());
    pDevice->CreateShaderResourceView(_resource[A8h_rhs].Get(), nullptr, _srv[A8h_rhs].ReleaseAndGetAddressOf());

	// D8h
    pDevice->CreateTexture3D(&desc, nullptr, _resource[D8h].ReleaseAndGetAddressOf());
    pDevice->CreateUnorderedAccessView(_resource[D8h].Get(), nullptr, _uav[D8h].ReleaseAndGetAddressOf());
    pDevice->CreateShaderResourceView(_resource[D8h].Get(), nullptr, _srv[D8h].ReleaseAndGetAddressOf());

	// A16h
	desc.Width = gSimulationInfo.width >> 4;
	desc.Height = gSimulationInfo.height >> 4;
	desc.Depth = gSimulationInfo.depth >> 4;
    pDevice->CreateTexture3D(&desc, nullptr, _resource[A16h].ReleaseAndGetAddressOf());
    pDevice->CreateUnorderedAccessView(_resource[A16h].Get(), nullptr, _uav[A16h].ReleaseAndGetAddressOf());
    pDevice->CreateShaderResourceView(_resource[A16h].Get(), nullptr, _srv[A16h].ReleaseAndGetAddressOf());

	// A16h_temp
    pDevice->CreateTexture3D(&desc, nullptr, _resource[A16h_temp].ReleaseAndGetAddressOf());
    pDevice->CreateUnorderedAccessView(_resource[A16h_temp].Get(), nullptr, _uav[A16h_temp].ReleaseAndGetAddressOf());
    pDevice->CreateShaderResourceView(_resource[A16h_temp].Get(), nullptr, _srv[A16h_temp].ReleaseAndGetAddressOf());

	// A16h_residual
    pDevice->CreateTexture3D(&desc, nullptr, _resource[A16h_residual].ReleaseAndGetAddressOf());
    pDevice->CreateUnorderedAccessView(_resource[A16h_residual].Get(), nullptr, _uav[A16h_residual].ReleaseAndGetAddressOf());
    pDevice->CreateShaderResourceView(_resource[A16h_residual].Get(), nullptr, _srv[A16h_residual].ReleaseAndGetAddressOf());

	// A16h_rhs
    pDevice->CreateTexture3D(&desc, nullptr, _resource[A16h_rhs].ReleaseAndGetAddressOf());
    pDevice->CreateUnorderedAccessView(_resource[A16h_rhs].Get(), nullptr, _uav[A16h_rhs].ReleaseAndGetAddressOf());
    pDevice->CreateShaderResourceView(_resource[A16h_rhs].Get(), nullptr, _srv[A16h_rhs].ReleaseAndGetAddressOf());

	// D16h
    pDevice->CreateTexture3D(&desc, nullptr, _resource[D16h].ReleaseAndGetAddressOf());
    pDevice->CreateUnorderedAccessView(_resource[D16h].Get(), nullptr, _uav[D16h].ReleaseAndGetAddressOf());
    pDevice->CreateShaderResourceView(_resource[D16h].Get(), nullptr, _srv[D16h].ReleaseAndGetAddressOf());

	// A32h
	desc.Width = gSimulationInfo.width >> 5;
	desc.Height = gSimulationInfo.height >> 5;
	desc.Depth = gSimulationInfo.depth >> 5;
    pDevice->CreateTexture3D(&desc, nullptr, _resource[A32h].ReleaseAndGetAddressOf());
    pDevice->CreateUnorderedAccessView(_resource[A32h].Get(), nullptr, _uav[A32h].ReleaseAndGetAddressOf());
    pDevice->CreateShaderResourceView(_resource[A32h].Get(), nullptr, _srv[A32h].ReleaseAndGetAddressOf());

	// A32h_temp
    pDevice->CreateTexture3D(&desc, nullptr, _resource[A32h_temp].ReleaseAndGetAddressOf());
    pDevice->CreateUnorderedAccessView(_resource[A32h_temp].Get(), nullptr, _uav[A32h_temp].ReleaseAndGetAddressOf());
    pDevice->CreateShaderResourceView(_resource[A32h_temp].Get(), nullptr, _srv[A32h_temp].ReleaseAndGetAddressOf());

	// A32h_residual
    pDevice->CreateTexture3D(&desc, nullptr, _resource[A32h_residual].ReleaseAndGetAddressOf());
    pDevice->CreateUnorderedAccessView(_resource[A32h_residual].Get(), nullptr, _uav[A32h_residual].ReleaseAndGetAddressOf());
    pDevice->CreateShaderResourceView(_resource[A32h_residual].Get(), nullptr, _srv[A32h_residual].ReleaseAndGetAddressOf());

	// A32h_rhs
    pDevice->CreateTexture3D(&desc, nullptr, _resource[A32h_rhs].ReleaseAndGetAddressOf());
    pDevice->CreateUnorderedAccessView(_resource[A32h_rhs].Get(), nullptr, _uav[A32h_rhs].ReleaseAndGetAddressOf());
    pDevice->CreateShaderResourceView(_resource[A32h_rhs].Get(), nullptr, _srv[A32h_rhs].ReleaseAndGetAddressOf());

	// D32h
    pDevice->CreateTexture3D(&desc, nullptr, _resource[D32h].ReleaseAndGetAddressOf());
    pDevice->CreateUnorderedAccessView(_resource[D32h].Get(), nullptr, _uav[D32h].ReleaseAndGetAddressOf());
    pDevice->CreateShaderResourceView(_resource[D32h].Get(), nullptr, _srv[D32h].ReleaseAndGetAddressOf());

	// A64h
	desc.Width = gSimulationInfo.width >> 6;
	desc.Height = gSimulationInfo.height >> 6;
	desc.Depth = gSimulationInfo.depth >> 6;
    pDevice->CreateTexture3D(&desc, nullptr, _resource[A64h].ReleaseAndGetAddressOf());
    pDevice->CreateUnorderedAccessView(_resource[A64h].Get(), nullptr, _uav[A64h].ReleaseAndGetAddressOf());
    pDevice->CreateShaderResourceView(_resource[A64h].Get(), nullptr, _srv[A64h].ReleaseAndGetAddressOf());

	// A64h_temp
    pDevice->CreateTexture3D(&desc, nullptr, _resource[A64h_temp].ReleaseAndGetAddressOf());
    pDevice->CreateUnorderedAccessView(_resource[A64h_temp].Get(), nullptr, _uav[A64h_temp].ReleaseAndGetAddressOf());
    pDevice->CreateShaderResourceView(_resource[A64h_temp].Get(), nullptr, _srv[A64h_temp].ReleaseAndGetAddressOf());

	// A64h_residual
    pDevice->CreateTexture3D(&desc, nullptr, _resource[A64h_residual].ReleaseAndGetAddressOf());
    pDevice->CreateUnorderedAccessView(_resource[A64h_residual].Get(), nullptr, _uav[A64h_residual].ReleaseAndGetAddressOf());
    pDevice->CreateShaderResourceView(_resource[A64h_residual].Get(), nullptr, _srv[A64h_residual].ReleaseAndGetAddressOf());

	// A64h_rhs
    pDevice->CreateTexture3D(&desc, nullptr, _resource[A64h_rhs].ReleaseAndGetAddressOf());
    pDevice->CreateUnorderedAccessView(_resource[A64h_rhs].Get(), nullptr, _uav[A64h_rhs].ReleaseAndGetAddressOf());
    pDevice->CreateShaderResourceView(_resource[A64h_rhs].Get(), nullptr, _srv[A64h_rhs].ReleaseAndGetAddressOf());

	// D64h
    pDevice->CreateTexture3D(&desc, nullptr, _resource[D64h].ReleaseAndGetAddressOf());
    pDevice->CreateUnorderedAccessView(_resource[D64h].Get(), nullptr, _uav[D64h].ReleaseAndGetAddressOf());
    pDevice->CreateShaderResourceView(_resource[D64h].Get(), nullptr, _srv[D64h].ReleaseAndGetAddressOf());

	// Final Result
	desc.Width = gSimulationInfo.width;
	desc.Height = gSimulationInfo.height;
	desc.Depth = gSimulationInfo.depth;
    pDevice->CreateTexture3D(&desc, nullptr, _resource[Final].ReleaseAndGetAddressOf());
    pDevice->CreateUnorderedAccessView(_resource[Final].Get(), nullptr, _uav[Final].ReleaseAndGetAddressOf());
    pDevice->CreateShaderResourceView(_resource[Final].Get(), nullptr, _srv[Final].ReleaseAndGetAddressOf());

    _divID = NodeManager::IssueIncomingAttrID();
    _incoming[_divID] = -1;
    _attrNames[_divID] = { "Div" };

    _pressureOutID = NodeManager::IssueOutgoingAttrID();
    _outgoing[_pressureOutID] = -1;
    _attrNames[_pressureOutID] = { "Pressure" };

    {
		std::string _path{"./CSO/MultiGridInit_CS.cso"};
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
		std::string _path{"./CSO/MultiGridJacobi_CS.cso"};
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
			_mgJacobiCS.ReleaseAndGetAddressOf()
		);
    }

    {
		std::string _path{"./CSO/Residual3D_CS.cso"};
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
			_residualCS.ReleaseAndGetAddressOf()
		);
    }

    {
		std::string _path{"./CSO/Subtract3D_CS.cso"};
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
			_subtractCS.ReleaseAndGetAddressOf()
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

	// initial guess 
	ID3D11UnorderedAccessView* uav[7]{ _uav[Ah].Get(), _uav[A2h].Get(), _uav[A4h].Get(), _uav[A8h].Get(), _uav[A16h].Get(), _uav[A32h].Get(), _uav[A64h].Get() };
	context.CSSetUnorderedAccessViews(0u, 7u, uav, nullptr);
	context.CSSetShader(_initCS.Get(), nullptr, 0u);
	context.Dispatch(
		static_cast<UINT>(ceil(static_cast<float>(gSimulationInfo.width) / _groupX)),
		static_cast<UINT>(ceil(static_cast<float>(gSimulationInfo.height) / _groupY)), 
		static_cast<UINT>(ceil(static_cast<float>(gSimulationInfo.depth) / _groupZ)) 
	);
	SetBarrier(context);

	// Jacobi for Ah
	{
		ID3D11ShaderResourceView* srv[2]{ _srv[Ah].Get(), _srv[Dh].Get() };
		context.CSSetShaderResources(0u, 2u, srv);
		context.CSSetUnorderedAccessViews(0u, 1u, _uav[Ah_temp].GetAddressOf(), nullptr);
		context.CSSetShader(_cs.Get(), nullptr, 0u);
		context.Dispatch(
			static_cast<UINT>(ceil(static_cast<float>(gSimulationInfo.width)  / _groupX)),
			static_cast<UINT>(ceil(static_cast<float>(gSimulationInfo.height) / _groupY)), 
			static_cast<UINT>(ceil(static_cast<float>(gSimulationInfo.depth)  / _groupZ)) 
		);
		SetBarrier(context);

		srv[0] = _srv[Ah_temp].Get();
		srv[1] = _srv[Dh].Get();
		context.CSSetShaderResources(0u, 2u, srv);
		context.CSSetUnorderedAccessViews(0u, 1u, _uav[Ah].GetAddressOf(), nullptr);
		context.CSSetShader(_cs.Get(), nullptr, 0u);
		context.Dispatch(
			static_cast<UINT>(ceil(static_cast<float>(gSimulationInfo.width)  / _groupX)),
			static_cast<UINT>(ceil(static_cast<float>(gSimulationInfo.height) / _groupY)), 
			static_cast<UINT>(ceil(static_cast<float>(gSimulationInfo.depth)  / _groupZ)) 
		);
		SetBarrier(context);

		srv[0] = _srv[Ah].Get();
		srv[1] = _srv[Dh].Get();
		context.CSSetShaderResources(0u, 2u, srv);
		context.CSSetUnorderedAccessViews(0u, 1u, _uav[Ah_temp].GetAddressOf(), nullptr);
		context.CSSetShader(_cs.Get(), nullptr, 0u);
		context.Dispatch(
			static_cast<UINT>(ceil(static_cast<float>(gSimulationInfo.width)  / _groupX)),
			static_cast<UINT>(ceil(static_cast<float>(gSimulationInfo.height) / _groupY)), 
			static_cast<UINT>(ceil(static_cast<float>(gSimulationInfo.depth)  / _groupZ)) 
		);
		SetBarrier(context);
	}

	// Residual Ah
	ID3D11ShaderResourceView* srv[2]{ _srv[Ah_temp].Get(), _srv[Dh].Get() };
	context.CSSetShaderResources(0u, 2u, srv);
	context.CSSetUnorderedAccessViews(0u, 1u, _uav[Ah_residual].GetAddressOf(), nullptr);
	context.CSSetShader(_residualCS.Get(), nullptr, 0u);
	context.Dispatch(
		static_cast<UINT>(ceil(static_cast<float>(gSimulationInfo.width)  / _groupX)),
		static_cast<UINT>(ceil(static_cast<float>(gSimulationInfo.height) / _groupY)), 
		static_cast<UINT>(ceil(static_cast<float>(gSimulationInfo.depth)  / _groupZ)) 
	);
	SetBarrier(context);

	// Restrict Ah to A2h
	context.CSSetShaderResources(0u, 1u, _srv[Ah_temp].GetAddressOf());
	context.CSSetUnorderedAccessViews(0u, 1u, _uav[A2h].GetAddressOf(), nullptr);
	context.CSSetShader(_restrictCS.Get(), nullptr, 0u);
	context.Dispatch(
		static_cast<UINT>(ceil(static_cast<float>(gSimulationInfo.width  >> 1) / _groupX)),
		static_cast<UINT>(ceil(static_cast<float>(gSimulationInfo.height >> 1) / _groupY)), 
		static_cast<UINT>(ceil(static_cast<float>(gSimulationInfo.depth  >> 1) / _groupZ)) 
	);
	SetBarrier(context);

	// RHS Ah
	context.CSSetShaderResources(0u, 1u, _srv[Ah_residual].GetAddressOf());
	context.CSSetShaderResources(1u, 1u, _srv[Ah_temp].GetAddressOf());
	context.CSSetUnorderedAccessViews(0u, 1u, _uav[Ah_rhs].GetAddressOf(), nullptr);
	context.CSSetShader(_subtractCS.Get(), nullptr, 0u);
	context.Dispatch(
		static_cast<UINT>(ceil(static_cast<float>(gSimulationInfo.width)  / _groupX)),
		static_cast<UINT>(ceil(static_cast<float>(gSimulationInfo.height) / _groupY)), 
		static_cast<UINT>(ceil(static_cast<float>(gSimulationInfo.depth)  / _groupZ)) 
	);
	SetBarrier(context);

	context.CSSetShaderResources(0u, 1u, _srv[Ah_rhs].GetAddressOf());
	context.CSSetUnorderedAccessViews(0u, 1u, _uav[D2h].GetAddressOf(), nullptr);
	context.CSSetShader(_restrictCS.Get(), nullptr, 0u);
	context.Dispatch(
		static_cast<UINT>(ceil(static_cast<float>(gSimulationInfo.width  >> 1) / _groupX)),
		static_cast<UINT>(ceil(static_cast<float>(gSimulationInfo.height >> 1) / _groupY)), 
		static_cast<UINT>(ceil(static_cast<float>(gSimulationInfo.depth  >> 1) / _groupZ)) 
	);
	SetBarrier(context);

	// Jacobi for A2h
	{
		context.CSSetShaderResources(0u, 1u, _srv[A2h].GetAddressOf());
		context.CSSetShaderResources(1u, 1u, _srv[D2h].GetAddressOf());
		context.CSSetUnorderedAccessViews(0u, 1u, _uav[A2h_temp].GetAddressOf(), nullptr);
		context.CSSetShader(_mgJacobiCS.Get(), nullptr, 0u);
		context.Dispatch(
			static_cast<UINT>(ceil(static_cast<float>(gSimulationInfo.width  >> 1) / _groupX)),
			static_cast<UINT>(ceil(static_cast<float>(gSimulationInfo.height >> 1) / _groupY)), 
			static_cast<UINT>(ceil(static_cast<float>(gSimulationInfo.depth  >> 1) / _groupZ)) 
		);
		SetBarrier(context);

		context.CSSetShaderResources(0u, 1u, _srv[A2h_temp].GetAddressOf());
		context.CSSetShaderResources(1u, 1u, _srv[D2h].GetAddressOf());
		context.CSSetUnorderedAccessViews(0u, 1u, _uav[A2h].GetAddressOf(), nullptr);
		context.CSSetShader(_mgJacobiCS.Get(), nullptr, 0u);
		context.Dispatch(
			static_cast<UINT>(ceil(static_cast<float>(gSimulationInfo.width  >> 1) / _groupX)),
			static_cast<UINT>(ceil(static_cast<float>(gSimulationInfo.height >> 1) / _groupY)), 
			static_cast<UINT>(ceil(static_cast<float>(gSimulationInfo.depth  >> 1) / _groupZ)) 
		);
		SetBarrier(context);

		context.CSSetShaderResources(0u, 1u, _srv[A2h].GetAddressOf());
		context.CSSetShaderResources(1u, 1u, _srv[D2h].GetAddressOf());
		context.CSSetUnorderedAccessViews(0u, 1u, _uav[A2h_temp].GetAddressOf(), nullptr);
		context.CSSetShader(_mgJacobiCS.Get(), nullptr, 0u);
		context.Dispatch(
			static_cast<UINT>(ceil(static_cast<float>(gSimulationInfo.width  >> 1) / _groupX)),
			static_cast<UINT>(ceil(static_cast<float>(gSimulationInfo.height >> 1) / _groupY)), 
			static_cast<UINT>(ceil(static_cast<float>(gSimulationInfo.depth  >> 1) / _groupZ)) 
		);
		SetBarrier(context);
	}

	// RHS A2h
	context.CSSetShaderResources(0u, 1u, _srv[Ah_rhs].GetAddressOf());
	context.CSSetUnorderedAccessViews(0u, 1u, _uav[A2h_int].GetAddressOf(), nullptr);
	context.CSSetShader(_restrictCS.Get(), nullptr, 0u);
	context.Dispatch(
		static_cast<UINT>(ceil(static_cast<float>(gSimulationInfo.width  >> 1) / _groupX)),
		static_cast<UINT>(ceil(static_cast<float>(gSimulationInfo.height >> 1) / _groupY)), 
		static_cast<UINT>(ceil(static_cast<float>(gSimulationInfo.depth  >> 1) / _groupZ)) 
	);
	SetBarrier(context);

	context.CSSetShaderResources(0u, 1u, _srv[A2h_int].GetAddressOf());
	context.CSSetShaderResources(1u, 1u, _srv[A2h_temp].GetAddressOf());
	context.CSSetUnorderedAccessViews(0u, 1u, _uav[A2h_rhs].GetAddressOf(), nullptr);
	context.CSSetShader(_subtractCS.Get(), nullptr, 0u);
	context.Dispatch(
		static_cast<UINT>(ceil(static_cast<float>(gSimulationInfo.width  >> 1) / _groupX)),
		static_cast<UINT>(ceil(static_cast<float>(gSimulationInfo.height >> 1) / _groupY)), 
		static_cast<UINT>(ceil(static_cast<float>(gSimulationInfo.depth  >> 1) / _groupZ)) 
	);
	SetBarrier(context);

	// Residual A2h
	context.CSSetShaderResources(0u, 1u, _srv[A2h_temp].GetAddressOf());
	context.CSSetShaderResources(1u, 1u, _srv[D2h].GetAddressOf());
	context.CSSetUnorderedAccessViews(0u, 1u, _uav[A2h_residual].GetAddressOf(), nullptr);
	context.CSSetShader(_residualCS.Get(), nullptr, 0u);
	context.Dispatch(
		static_cast<UINT>(ceil(static_cast<float>(gSimulationInfo.width  >> 1) / _groupX)),
		static_cast<UINT>(ceil(static_cast<float>(gSimulationInfo.height >> 1) / _groupY)), 
		static_cast<UINT>(ceil(static_cast<float>(gSimulationInfo.depth  >> 1) / _groupZ)) 
	);
	SetBarrier(context);

	// Restrict A2h to A4h
	context.CSSetShaderResources(0u, 1u, _srv[A2h_temp].GetAddressOf());
	context.CSSetUnorderedAccessViews(0u, 1u, _uav[A4h].GetAddressOf(), nullptr);
	context.CSSetShader(_restrictCS.Get(), nullptr, 0u);
	context.Dispatch(
		static_cast<UINT>(ceil(static_cast<float>(gSimulationInfo.width  >> 2) / _groupX)),
		static_cast<UINT>(ceil(static_cast<float>(gSimulationInfo.height >> 2) / _groupY)), 
		static_cast<UINT>(ceil(static_cast<float>(gSimulationInfo.depth  >> 2) / _groupZ)) 
	);
	SetBarrier(context);

	context.CSSetShaderResources(0u, 1u, _srv[A2h_rhs].GetAddressOf());
	context.CSSetUnorderedAccessViews(0u, 1u, _uav[D4h].GetAddressOf(), nullptr);
	context.CSSetShader(_restrictCS.Get(), nullptr, 0u);
	context.Dispatch(
		static_cast<UINT>(ceil(static_cast<float>(gSimulationInfo.width  >> 2) / _groupX)),
		static_cast<UINT>(ceil(static_cast<float>(gSimulationInfo.height >> 2) / _groupY)), 
		static_cast<UINT>(ceil(static_cast<float>(gSimulationInfo.depth  >> 2) / _groupZ)) 
	);
	SetBarrier(context);

	// Jacobi for A4h
	{
		context.CSSetShaderResources(0u, 1u, _srv[A4h].GetAddressOf());
		context.CSSetShaderResources(1u, 1u, _srv[D4h].GetAddressOf());
		context.CSSetUnorderedAccessViews(0u, 1u, _uav[A4h_temp].GetAddressOf(), nullptr);
		context.CSSetShader(_mgJacobiCS.Get(), nullptr, 0u);
		context.Dispatch(
			static_cast<UINT>(ceil(static_cast<float>(gSimulationInfo.width  >> 2) / _groupX)),
			static_cast<UINT>(ceil(static_cast<float>(gSimulationInfo.height >> 2) / _groupY)), 
			static_cast<UINT>(ceil(static_cast<float>(gSimulationInfo.depth  >> 2) / _groupZ)) 
		);
		SetBarrier(context);

		context.CSSetShaderResources(0u, 1u, _srv[A4h_temp].GetAddressOf());
		context.CSSetShaderResources(1u, 1u, _srv[D4h].GetAddressOf());
		context.CSSetUnorderedAccessViews(0u, 1u, _uav[A4h].GetAddressOf(), nullptr);
		context.CSSetShader(_mgJacobiCS.Get(), nullptr, 0u);
		context.Dispatch(
			static_cast<UINT>(ceil(static_cast<float>(gSimulationInfo.width  >> 2) / _groupX)),
			static_cast<UINT>(ceil(static_cast<float>(gSimulationInfo.height >> 2) / _groupY)), 
			static_cast<UINT>(ceil(static_cast<float>(gSimulationInfo.depth  >> 2) / _groupZ)) 
		);
		SetBarrier(context);

		context.CSSetShaderResources(0u, 1u, _srv[A4h].GetAddressOf());
		context.CSSetShaderResources(1u, 1u, _srv[D4h].GetAddressOf());
		context.CSSetUnorderedAccessViews(0u, 1u, _uav[A4h_temp].GetAddressOf(), nullptr);
		context.CSSetShader(_mgJacobiCS.Get(), nullptr, 0u);
		context.Dispatch(
			static_cast<UINT>(ceil(static_cast<float>(gSimulationInfo.width  >> 2) / _groupX)),
			static_cast<UINT>(ceil(static_cast<float>(gSimulationInfo.height >> 2) / _groupY)), 
			static_cast<UINT>(ceil(static_cast<float>(gSimulationInfo.depth  >> 2) / _groupZ)) 
		);
		SetBarrier(context);
	}

	// RHS A4h
	context.CSSetShaderResources(0u, 1u, _srv[A2h_residual].GetAddressOf());
	context.CSSetShaderResources(1u, 1u, _srv[A2h_rhs].GetAddressOf());
	context.CSSetUnorderedAccessViews(0u, 1u, _uav[A2h_int].GetAddressOf(), nullptr);
	context.CSSetShader(_subtractCS.Get(), nullptr, 0u);
	context.Dispatch(
		static_cast<UINT>(ceil(static_cast<float>(gSimulationInfo.width  >> 1) / _groupX)),
		static_cast<UINT>(ceil(static_cast<float>(gSimulationInfo.height >> 1) / _groupY)), 
		static_cast<UINT>(ceil(static_cast<float>(gSimulationInfo.depth  >> 1) / _groupZ)) 
	);
	SetBarrier(context);

	context.CSSetShaderResources(0u, 1u, _srv[A2h_int].GetAddressOf());
	context.CSSetUnorderedAccessViews(0u, 1u, _uav[A4h_int].GetAddressOf(), nullptr);
	context.CSSetShader(_restrictCS.Get(), nullptr, 0u);
	context.Dispatch(
		static_cast<UINT>(ceil(static_cast<float>(gSimulationInfo.width  >> 2) / _groupX)),
		static_cast<UINT>(ceil(static_cast<float>(gSimulationInfo.height >> 2) / _groupY)), 
		static_cast<UINT>(ceil(static_cast<float>(gSimulationInfo.depth  >> 2) / _groupZ)) 
	);
	SetBarrier(context);

	context.CSSetShaderResources(0u, 1u, _srv[A4h_int].GetAddressOf());
	context.CSSetShaderResources(1u, 1u, _srv[A4h_temp].GetAddressOf());
	context.CSSetUnorderedAccessViews(0u, 1u, _uav[A4h_rhs].GetAddressOf(), nullptr);
	context.CSSetShader(_subtractCS.Get(), nullptr, 0u);
	context.Dispatch(
		static_cast<UINT>(ceil(static_cast<float>(gSimulationInfo.width  >> 2) / _groupX)),
		static_cast<UINT>(ceil(static_cast<float>(gSimulationInfo.height >> 2) / _groupY)), 
		static_cast<UINT>(ceil(static_cast<float>(gSimulationInfo.depth  >> 2) / _groupZ)) 
	);
	SetBarrier(context);

	// Residual A4h
	context.CSSetShaderResources(0u, 1u, _srv[A4h_temp].GetAddressOf());
	context.CSSetShaderResources(1u, 1u, _srv[D4h].GetAddressOf());
	context.CSSetUnorderedAccessViews(0u, 1u, _uav[A4h_residual].GetAddressOf(), nullptr);
	context.CSSetShader(_residualCS.Get(), nullptr, 0u);
	context.Dispatch(
		static_cast<UINT>(ceil(static_cast<float>(gSimulationInfo.width  >> 2) / _groupX)),
		static_cast<UINT>(ceil(static_cast<float>(gSimulationInfo.height >> 2) / _groupY)), 
		static_cast<UINT>(ceil(static_cast<float>(gSimulationInfo.depth  >> 2) / _groupZ)) 
	);
	SetBarrier(context);

	// Restrict A4h to A8h
	context.CSSetShaderResources(0u, 1u, _srv[A4h_temp].GetAddressOf());
	context.CSSetUnorderedAccessViews(0u, 1u, _uav[A8h].GetAddressOf(), nullptr);
	context.CSSetShader(_restrictCS.Get(), nullptr, 0u);
	context.Dispatch(
		static_cast<UINT>(ceil(static_cast<float>(gSimulationInfo.width  >> 3) / _groupX)),
		static_cast<UINT>(ceil(static_cast<float>(gSimulationInfo.height >> 3) / _groupY)), 
		static_cast<UINT>(ceil(static_cast<float>(gSimulationInfo.depth  >> 3) / _groupZ)) 
	);
	SetBarrier(context);

	context.CSSetShaderResources(0u, 1u, _srv[A4h_rhs].GetAddressOf());
	context.CSSetUnorderedAccessViews(0u, 1u, _uav[D8h].GetAddressOf(), nullptr);
	context.CSSetShader(_restrictCS.Get(), nullptr, 0u);
	context.Dispatch(
		static_cast<UINT>(ceil(static_cast<float>(gSimulationInfo.width  >> 3) / _groupX)),
		static_cast<UINT>(ceil(static_cast<float>(gSimulationInfo.height >> 3) / _groupY)), 
		static_cast<UINT>(ceil(static_cast<float>(gSimulationInfo.depth  >> 3) / _groupZ)) 
	);
	SetBarrier(context);

	// Jacobi for A8h
	{
		context.CSSetShaderResources(0u, 1u, _srv[A8h].GetAddressOf());
		context.CSSetShaderResources(1u, 1u, _srv[D8h].GetAddressOf());
		context.CSSetUnorderedAccessViews(0u, 1u, _uav[A8h_temp].GetAddressOf(), nullptr);
		context.CSSetShader(_mgJacobiCS.Get(), nullptr, 0u);
		context.Dispatch(
			static_cast<UINT>(ceil(static_cast<float>(gSimulationInfo.width  >> 3) / _groupX)),
			static_cast<UINT>(ceil(static_cast<float>(gSimulationInfo.height >> 3) / _groupY)), 
			static_cast<UINT>(ceil(static_cast<float>(gSimulationInfo.depth  >> 3) / _groupZ)) 
		);
		SetBarrier(context);

		context.CSSetShaderResources(0u, 1u, _srv[A8h_temp].GetAddressOf());
		context.CSSetShaderResources(1u, 1u, _srv[D8h].GetAddressOf());
		context.CSSetUnorderedAccessViews(0u, 1u, _uav[A8h].GetAddressOf(), nullptr);
		context.CSSetShader(_mgJacobiCS.Get(), nullptr, 0u);
		context.Dispatch(
			static_cast<UINT>(ceil(static_cast<float>(gSimulationInfo.width  >> 3) / _groupX)),
			static_cast<UINT>(ceil(static_cast<float>(gSimulationInfo.height >> 3) / _groupY)), 
			static_cast<UINT>(ceil(static_cast<float>(gSimulationInfo.depth  >> 3) / _groupZ)) 
		);
		SetBarrier(context);

		context.CSSetShaderResources(0u, 1u, _srv[A8h].GetAddressOf());
		context.CSSetShaderResources(1u, 1u, _srv[D8h].GetAddressOf());
		context.CSSetUnorderedAccessViews(0u, 1u, _uav[A8h_temp].GetAddressOf(), nullptr);
		context.CSSetShader(_mgJacobiCS.Get(), nullptr, 0u);
		context.Dispatch(
			static_cast<UINT>(ceil(static_cast<float>(gSimulationInfo.width  >> 3) / _groupX)),
			static_cast<UINT>(ceil(static_cast<float>(gSimulationInfo.height >> 3) / _groupY)), 
			static_cast<UINT>(ceil(static_cast<float>(gSimulationInfo.depth  >> 3) / _groupZ)) 
		);
		SetBarrier(context);
	}

	// RHS A8h
	context.CSSetShaderResources(0u, 1u, _srv[A4h_residual].GetAddressOf());
	context.CSSetShaderResources(1u, 1u, _srv[A4h_rhs].GetAddressOf());
	context.CSSetUnorderedAccessViews(0u, 1u, _uav[A4h_int].GetAddressOf(), nullptr);
	context.CSSetShader(_subtractCS.Get(), nullptr, 0u);
	context.Dispatch(
		static_cast<UINT>(ceil(static_cast<float>(gSimulationInfo.width  >> 2) / _groupX)),
		static_cast<UINT>(ceil(static_cast<float>(gSimulationInfo.height >> 2) / _groupY)), 
		static_cast<UINT>(ceil(static_cast<float>(gSimulationInfo.depth  >> 2) / _groupZ)) 
	);
	SetBarrier(context);

	context.CSSetShaderResources(0u, 1u, _srv[A4h_int].GetAddressOf());
	context.CSSetUnorderedAccessViews(0u, 1u, _uav[A8h_int].GetAddressOf(), nullptr);
	context.CSSetShader(_restrictCS.Get(), nullptr, 0u);
	context.Dispatch(
		static_cast<UINT>(ceil(static_cast<float>(gSimulationInfo.width  >> 3) / _groupX)),
		static_cast<UINT>(ceil(static_cast<float>(gSimulationInfo.height >> 3) / _groupY)), 
		static_cast<UINT>(ceil(static_cast<float>(gSimulationInfo.depth  >> 3) / _groupZ)) 
	);
	SetBarrier(context);

	context.CSSetShaderResources(0u, 1u, _srv[A8h_int].GetAddressOf());
	context.CSSetShaderResources(1u, 1u, _srv[A8h_temp].GetAddressOf());
	context.CSSetUnorderedAccessViews(0u, 1u, _uav[A8h_rhs].GetAddressOf(), nullptr);
	context.CSSetShader(_subtractCS.Get(), nullptr, 0u);
	context.Dispatch(
		static_cast<UINT>(ceil(static_cast<float>(gSimulationInfo.width  >> 3) / _groupX)),
		static_cast<UINT>(ceil(static_cast<float>(gSimulationInfo.height >> 3) / _groupY)), 
		static_cast<UINT>(ceil(static_cast<float>(gSimulationInfo.depth  >> 3) / _groupZ)) 
	);
	SetBarrier(context);

	// Residual A8h
	context.CSSetShaderResources(0u, 1u, _srv[A8h_temp].GetAddressOf());
	context.CSSetShaderResources(1u, 1u, _srv[D8h].GetAddressOf());
	context.CSSetUnorderedAccessViews(0u, 1u, _uav[A8h_residual].GetAddressOf(), nullptr);
	context.CSSetShader(_residualCS.Get(), nullptr, 0u);
	context.Dispatch(
		static_cast<UINT>(ceil(static_cast<float>(gSimulationInfo.width  >> 3) / _groupX)),
		static_cast<UINT>(ceil(static_cast<float>(gSimulationInfo.height >> 3) / _groupY)), 
		static_cast<UINT>(ceil(static_cast<float>(gSimulationInfo.depth  >> 3) / _groupZ)) 
	);
	SetBarrier(context);

	// Restrict A8h to A16h
	context.CSSetShaderResources(0u, 1u, _srv[A8h_temp].GetAddressOf());
	context.CSSetUnorderedAccessViews(0u, 1u, _uav[A16h].GetAddressOf(), nullptr);
	context.CSSetShader(_restrictCS.Get(), nullptr, 0u);
	context.Dispatch(
		static_cast<UINT>(ceil(static_cast<float>(gSimulationInfo.width  >> 4) / _groupX)),
		static_cast<UINT>(ceil(static_cast<float>(gSimulationInfo.height >> 4) / _groupY)), 
		static_cast<UINT>(ceil(static_cast<float>(gSimulationInfo.depth  >> 4) / _groupZ)) 
	);
	SetBarrier(context);

	context.CSSetShaderResources(0u, 1u, _srv[A8h_rhs].GetAddressOf());
	context.CSSetUnorderedAccessViews(0u, 1u, _uav[D16h].GetAddressOf(), nullptr);
	context.CSSetShader(_restrictCS.Get(), nullptr, 0u);
	context.Dispatch(
		static_cast<UINT>(ceil(static_cast<float>(gSimulationInfo.width  >> 4) / _groupX)),
		static_cast<UINT>(ceil(static_cast<float>(gSimulationInfo.height >> 4) / _groupY)), 
		static_cast<UINT>(ceil(static_cast<float>(gSimulationInfo.depth  >> 4) / _groupZ)) 
	);
	SetBarrier(context);

	// Jacobi for A16h
	{
		context.CSSetShaderResources(0u, 1u, _srv[A16h].GetAddressOf());
		context.CSSetShaderResources(1u, 1u, _srv[D16h].GetAddressOf());
		context.CSSetUnorderedAccessViews(0u, 1u, _uav[A16h_temp].GetAddressOf(), nullptr);
		context.CSSetShader(_mgJacobiCS.Get(), nullptr, 0u);
		context.Dispatch(
			static_cast<UINT>(ceil(static_cast<float>(gSimulationInfo.width  >> 4) / _groupX)),
			static_cast<UINT>(ceil(static_cast<float>(gSimulationInfo.height >> 4) / _groupY)), 
			static_cast<UINT>(ceil(static_cast<float>(gSimulationInfo.depth  >> 4) / _groupZ)) 
		);
		SetBarrier(context);

		context.CSSetShaderResources(0u, 1u, _srv[A16h_temp].GetAddressOf());
		context.CSSetShaderResources(1u, 1u, _srv[D16h].GetAddressOf());
		context.CSSetUnorderedAccessViews(0u, 1u, _uav[A16h].GetAddressOf(), nullptr);
		context.CSSetShader(_mgJacobiCS.Get(), nullptr, 0u);
		context.Dispatch(
			static_cast<UINT>(ceil(static_cast<float>(gSimulationInfo.width  >> 4) / _groupX)),
			static_cast<UINT>(ceil(static_cast<float>(gSimulationInfo.height >> 4) / _groupY)), 
			static_cast<UINT>(ceil(static_cast<float>(gSimulationInfo.depth  >> 4) / _groupZ)) 
		);
		SetBarrier(context);

		context.CSSetShaderResources(0u, 1u, _srv[A16h].GetAddressOf());
		context.CSSetShaderResources(1u, 1u, _srv[D16h].GetAddressOf());
		context.CSSetUnorderedAccessViews(0u, 1u, _uav[A16h_temp].GetAddressOf(), nullptr);
		context.CSSetShader(_mgJacobiCS.Get(), nullptr, 0u);
		context.Dispatch(
			static_cast<UINT>(ceil(static_cast<float>(gSimulationInfo.width  >> 4) / _groupX)),
			static_cast<UINT>(ceil(static_cast<float>(gSimulationInfo.height >> 4) / _groupY)), 
			static_cast<UINT>(ceil(static_cast<float>(gSimulationInfo.depth  >> 4) / _groupZ)) 
		);
		SetBarrier(context);
	}

	// RHS A16h
	context.CSSetShaderResources(0u, 1u, _srv[A8h_residual].GetAddressOf());
	context.CSSetShaderResources(1u, 1u, _srv[A8h_rhs].GetAddressOf());
	context.CSSetUnorderedAccessViews(0u, 1u, _uav[A8h_int].GetAddressOf(), nullptr);
	context.CSSetShader(_subtractCS.Get(), nullptr, 0u);
	context.Dispatch(
		static_cast<UINT>(ceil(static_cast<float>(gSimulationInfo.width  >> 3) / _groupX)),
		static_cast<UINT>(ceil(static_cast<float>(gSimulationInfo.height >> 3) / _groupY)), 
		static_cast<UINT>(ceil(static_cast<float>(gSimulationInfo.depth  >> 3) / _groupZ)) 
	);
	SetBarrier(context);

	context.CSSetShaderResources(0u, 1u, _srv[A8h_int].GetAddressOf());
	context.CSSetUnorderedAccessViews(0u, 1u, _uav[A16h_int].GetAddressOf(), nullptr);
	context.CSSetShader(_restrictCS.Get(), nullptr, 0u);
	context.Dispatch(
		static_cast<UINT>(ceil(static_cast<float>(gSimulationInfo.width  >> 4) / _groupX)),
		static_cast<UINT>(ceil(static_cast<float>(gSimulationInfo.height >> 4) / _groupY)), 
		static_cast<UINT>(ceil(static_cast<float>(gSimulationInfo.depth  >> 4) / _groupZ)) 
	);
	SetBarrier(context);

	context.CSSetShaderResources(0u, 1u, _srv[A16h_int].GetAddressOf());
	context.CSSetShaderResources(1u, 1u, _srv[A16h_temp].GetAddressOf());
	context.CSSetUnorderedAccessViews(0u, 1u, _uav[A16h_rhs].GetAddressOf(), nullptr);
	context.CSSetShader(_subtractCS.Get(), nullptr, 0u);
	context.Dispatch(
		static_cast<UINT>(ceil(static_cast<float>(gSimulationInfo.width  >> 4) / _groupX)),
		static_cast<UINT>(ceil(static_cast<float>(gSimulationInfo.height >> 4) / _groupY)), 
		static_cast<UINT>(ceil(static_cast<float>(gSimulationInfo.depth  >> 4) / _groupZ)) 
	);
	SetBarrier(context);

	// Residual A16h
	context.CSSetShaderResources(0u, 1u, _srv[A16h_temp].GetAddressOf());
	context.CSSetShaderResources(1u, 1u, _srv[D16h].GetAddressOf());
	context.CSSetUnorderedAccessViews(0u, 1u, _uav[A16h_residual].GetAddressOf(), nullptr);
	context.CSSetShader(_residualCS.Get(), nullptr, 0u);
	context.Dispatch(
		static_cast<UINT>(ceil(static_cast<float>(gSimulationInfo.width  >> 4) / _groupX)),
		static_cast<UINT>(ceil(static_cast<float>(gSimulationInfo.height >> 4) / _groupY)), 
		static_cast<UINT>(ceil(static_cast<float>(gSimulationInfo.depth  >> 4) / _groupZ)) 
	);
	SetBarrier(context);

	// Restrict A16h to A32h
	context.CSSetShaderResources(0u, 1u, _srv[A16h_temp].GetAddressOf());
	context.CSSetUnorderedAccessViews(0u, 1u, _uav[A32h].GetAddressOf(), nullptr);
	context.CSSetShader(_restrictCS.Get(), nullptr, 0u);
	context.Dispatch(
		static_cast<UINT>(ceil(static_cast<float>(gSimulationInfo.width  >> 5) / _groupX)),
		static_cast<UINT>(ceil(static_cast<float>(gSimulationInfo.height >> 5) / _groupY)), 
		static_cast<UINT>(ceil(static_cast<float>(gSimulationInfo.depth  >> 5) / _groupZ)) 
	);
	SetBarrier(context);

	context.CSSetShaderResources(0u, 1u, _srv[A16h_rhs].GetAddressOf());
	context.CSSetUnorderedAccessViews(0u, 1u, _uav[D32h].GetAddressOf(), nullptr);
	context.CSSetShader(_restrictCS.Get(), nullptr, 0u);
	context.Dispatch(
		static_cast<UINT>(ceil(static_cast<float>(gSimulationInfo.width  >> 5) / _groupX)),
		static_cast<UINT>(ceil(static_cast<float>(gSimulationInfo.height >> 5) / _groupY)), 
		static_cast<UINT>(ceil(static_cast<float>(gSimulationInfo.depth  >> 5) / _groupZ)) 
	);
	SetBarrier(context);

	// Jacobi for A32h
	{
		context.CSSetShaderResources(0u, 1u, _srv[A32h].GetAddressOf());
		context.CSSetShaderResources(1u, 1u, _srv[D32h].GetAddressOf());
		context.CSSetUnorderedAccessViews(0u, 1u, _uav[A32h_temp].GetAddressOf(), nullptr);
		context.CSSetShader(_mgJacobiCS.Get(), nullptr, 0u);
		context.Dispatch(
			static_cast<UINT>(ceil(static_cast<float>(gSimulationInfo.width  >> 5) / _groupX)),
			static_cast<UINT>(ceil(static_cast<float>(gSimulationInfo.height >> 5) / _groupY)), 
			static_cast<UINT>(ceil(static_cast<float>(gSimulationInfo.depth  >> 5) / _groupZ)) 
		);
		SetBarrier(context);

		context.CSSetShaderResources(0u, 1u, _srv[A32h_temp].GetAddressOf());
		context.CSSetShaderResources(1u, 1u, _srv[D32h].GetAddressOf());
		context.CSSetUnorderedAccessViews(0u, 1u, _uav[A32h].GetAddressOf(), nullptr);
		context.CSSetShader(_mgJacobiCS.Get(), nullptr, 0u);
		context.Dispatch(
			static_cast<UINT>(ceil(static_cast<float>(gSimulationInfo.width  >> 5) / _groupX)),
			static_cast<UINT>(ceil(static_cast<float>(gSimulationInfo.height >> 5) / _groupY)), 
			static_cast<UINT>(ceil(static_cast<float>(gSimulationInfo.depth  >> 5) / _groupZ)) 
		);
		SetBarrier(context);

		context.CSSetShaderResources(0u, 1u, _srv[A32h].GetAddressOf());
		context.CSSetShaderResources(1u, 1u, _srv[D32h].GetAddressOf());
		context.CSSetUnorderedAccessViews(0u, 1u, _uav[A32h_temp].GetAddressOf(), nullptr);
		context.CSSetShader(_mgJacobiCS.Get(), nullptr, 0u);
		context.Dispatch(
			static_cast<UINT>(ceil(static_cast<float>(gSimulationInfo.width  >> 5) / _groupX)),
			static_cast<UINT>(ceil(static_cast<float>(gSimulationInfo.height >> 5) / _groupY)), 
			static_cast<UINT>(ceil(static_cast<float>(gSimulationInfo.depth  >> 5) / _groupZ)) 
		);
		SetBarrier(context);
	}

	// RHS A32h
	context.CSSetShaderResources(0u, 1u, _srv[A16h_residual].GetAddressOf());
	context.CSSetShaderResources(1u, 1u, _srv[A16h_rhs].GetAddressOf());
	context.CSSetUnorderedAccessViews(0u, 1u, _uav[A16h_int].GetAddressOf(), nullptr);
	context.CSSetShader(_subtractCS.Get(), nullptr, 0u);
	context.Dispatch(
		static_cast<UINT>(ceil(static_cast<float>(gSimulationInfo.width  >> 4) / _groupX)),
		static_cast<UINT>(ceil(static_cast<float>(gSimulationInfo.height >> 4) / _groupY)), 
		static_cast<UINT>(ceil(static_cast<float>(gSimulationInfo.depth  >> 4) / _groupZ)) 
	);
	SetBarrier(context);

	context.CSSetShaderResources(0u, 1u, _srv[A16h_int].GetAddressOf());
	context.CSSetUnorderedAccessViews(0u, 1u, _uav[A32h_int].GetAddressOf(), nullptr);
	context.CSSetShader(_restrictCS.Get(), nullptr, 0u);
	context.Dispatch(
		static_cast<UINT>(ceil(static_cast<float>(gSimulationInfo.width  >> 5) / _groupX)),
		static_cast<UINT>(ceil(static_cast<float>(gSimulationInfo.height >> 5) / _groupY)), 
		static_cast<UINT>(ceil(static_cast<float>(gSimulationInfo.depth  >> 5) / _groupZ)) 
	);
	SetBarrier(context);

	context.CSSetShaderResources(0u, 1u, _srv[A32h_int].GetAddressOf());
	context.CSSetShaderResources(1u, 1u, _srv[A32h_temp].GetAddressOf());
	context.CSSetUnorderedAccessViews(0u, 1u, _uav[A32h_rhs].GetAddressOf(), nullptr);
	context.CSSetShader(_subtractCS.Get(), nullptr, 0u);
	context.Dispatch(
		static_cast<UINT>(ceil(static_cast<float>(gSimulationInfo.width  >> 5) / _groupX)),
		static_cast<UINT>(ceil(static_cast<float>(gSimulationInfo.height >> 5) / _groupY)), 
		static_cast<UINT>(ceil(static_cast<float>(gSimulationInfo.depth  >> 5) / _groupZ)) 
	);
	SetBarrier(context);

	// Residual A32h
	context.CSSetShaderResources(0u, 1u, _srv[A32h_temp].GetAddressOf());
	context.CSSetShaderResources(1u, 1u, _srv[D32h].GetAddressOf());
	context.CSSetUnorderedAccessViews(0u, 1u, _uav[A32h_residual].GetAddressOf(), nullptr);
	context.CSSetShader(_residualCS.Get(), nullptr, 0u);
	context.Dispatch(
		static_cast<UINT>(ceil(static_cast<float>(gSimulationInfo.width  >> 5) / _groupX)),
		static_cast<UINT>(ceil(static_cast<float>(gSimulationInfo.height >> 5) / _groupY)), 
		static_cast<UINT>(ceil(static_cast<float>(gSimulationInfo.depth  >> 5) / _groupZ)) 
	);
	SetBarrier(context);

	// Restrict A32h to A64h
	context.CSSetShaderResources(0u, 1u, _srv[A32h_temp].GetAddressOf());
	context.CSSetUnorderedAccessViews(0u, 1u, _uav[A64h].GetAddressOf(), nullptr);
	context.CSSetShader(_restrictCS.Get(), nullptr, 0u);
	context.Dispatch(
		static_cast<UINT>(ceil(static_cast<float>(gSimulationInfo.width  >> 6) / _groupX)),
		static_cast<UINT>(ceil(static_cast<float>(gSimulationInfo.height >> 6) / _groupY)), 
		static_cast<UINT>(ceil(static_cast<float>(gSimulationInfo.depth  >> 6) / _groupZ)) 
	);
	SetBarrier(context);

	context.CSSetShaderResources(0u, 1u, _srv[A32h_rhs].GetAddressOf());
	context.CSSetUnorderedAccessViews(0u, 1u, _uav[D64h].GetAddressOf(), nullptr);
	context.CSSetShader(_restrictCS.Get(), nullptr, 0u);
	context.Dispatch(
		static_cast<UINT>(ceil(static_cast<float>(gSimulationInfo.width  >> 6) / _groupX)),
		static_cast<UINT>(ceil(static_cast<float>(gSimulationInfo.height >> 6) / _groupY)), 
		static_cast<UINT>(ceil(static_cast<float>(gSimulationInfo.depth  >> 6) / _groupZ)) 
	);
	SetBarrier(context);

	// Jacobi for A64h
	{
		context.CSSetShaderResources(0u, 1u, _srv[A64h].GetAddressOf());
		context.CSSetShaderResources(1u, 1u, _srv[D64h].GetAddressOf());
		context.CSSetUnorderedAccessViews(0u, 1u, _uav[A64h_temp].GetAddressOf(), nullptr);
		context.CSSetShader(_mgJacobiCS.Get(), nullptr, 0u);
		context.Dispatch(
			static_cast<UINT>(ceil(static_cast<float>(gSimulationInfo.width  >> 6) / _groupX)),
			static_cast<UINT>(ceil(static_cast<float>(gSimulationInfo.height >> 6) / _groupY)), 
			static_cast<UINT>(ceil(static_cast<float>(gSimulationInfo.depth  >> 6) / _groupZ)) 
		);
		SetBarrier(context);

		context.CSSetShaderResources(0u, 1u, _srv[A64h_temp].GetAddressOf());
		context.CSSetShaderResources(1u, 1u, _srv[D64h].GetAddressOf());
		context.CSSetUnorderedAccessViews(0u, 1u, _uav[A64h].GetAddressOf(), nullptr);
		context.CSSetShader(_mgJacobiCS.Get(), nullptr, 0u);
		context.Dispatch(
			static_cast<UINT>(ceil(static_cast<float>(gSimulationInfo.width  >> 6) / _groupX)),
			static_cast<UINT>(ceil(static_cast<float>(gSimulationInfo.height >> 6) / _groupY)), 
			static_cast<UINT>(ceil(static_cast<float>(gSimulationInfo.depth  >> 6) / _groupZ)) 
		);
		SetBarrier(context);

		context.CSSetShaderResources(0u, 1u, _srv[A64h].GetAddressOf());
		context.CSSetShaderResources(1u, 1u, _srv[D64h].GetAddressOf());
		context.CSSetUnorderedAccessViews(0u, 1u, _uav[A64h_temp].GetAddressOf(), nullptr);
		context.CSSetShader(_mgJacobiCS.Get(), nullptr, 0u);
		context.Dispatch(
			static_cast<UINT>(ceil(static_cast<float>(gSimulationInfo.width  >> 6) / _groupX)),
			static_cast<UINT>(ceil(static_cast<float>(gSimulationInfo.height >> 6) / _groupY)), 
			static_cast<UINT>(ceil(static_cast<float>(gSimulationInfo.depth  >> 6) / _groupZ)) 
		);
		SetBarrier(context);
	}

	// RHS A64h
	context.CSSetShaderResources(0u, 1u, _srv[A32h_residual].GetAddressOf());
	context.CSSetShaderResources(1u, 1u, _srv[A32h_rhs].GetAddressOf());
	context.CSSetUnorderedAccessViews(0u, 1u, _uav[A32h_int].GetAddressOf(), nullptr);
	context.CSSetShader(_subtractCS.Get(), nullptr, 0u);
	context.Dispatch(
		static_cast<UINT>(ceil(static_cast<float>(gSimulationInfo.width  >> 5) / _groupX)),
		static_cast<UINT>(ceil(static_cast<float>(gSimulationInfo.height >> 5) / _groupY)), 
		static_cast<UINT>(ceil(static_cast<float>(gSimulationInfo.depth  >> 5) / _groupZ)) 
	);
	SetBarrier(context);

	context.CSSetShaderResources(0u, 1u, _srv[A32h_int].GetAddressOf());
	context.CSSetUnorderedAccessViews(0u, 1u, _uav[A64h_int].GetAddressOf(), nullptr);
	context.CSSetShader(_restrictCS.Get(), nullptr, 0u);
	context.Dispatch(
		static_cast<UINT>(ceil(static_cast<float>(gSimulationInfo.width  >> 6) / _groupX)),
		static_cast<UINT>(ceil(static_cast<float>(gSimulationInfo.height >> 6) / _groupY)), 
		static_cast<UINT>(ceil(static_cast<float>(gSimulationInfo.depth  >> 6) / _groupZ)) 
	);
	SetBarrier(context);

	context.CSSetShaderResources(0u, 1u, _srv[A64h_int].GetAddressOf());
	context.CSSetShaderResources(1u, 1u, _srv[A64h_temp].GetAddressOf());
	context.CSSetUnorderedAccessViews(0u, 1u, _uav[A64h_rhs].GetAddressOf(), nullptr);
	context.CSSetShader(_subtractCS.Get(), nullptr, 0u);
	context.Dispatch(
		static_cast<UINT>(ceil(static_cast<float>(gSimulationInfo.width  >> 6) / _groupX)),
		static_cast<UINT>(ceil(static_cast<float>(gSimulationInfo.height >> 6) / _groupY)), 
		static_cast<UINT>(ceil(static_cast<float>(gSimulationInfo.depth  >> 6) / _groupZ)) 
	);
	SetBarrier(context);

	// Interpolate A64h to A32h
	context.CSSetShaderResources(0u, 1u, _srv[A64h_temp].GetAddressOf());
	context.CSSetUnorderedAccessViews(0u, 1u, _uav[A32h_temp].GetAddressOf(), nullptr);
	context.CSSetShader(_interpolateCS.Get(), nullptr, 0u);
	context.Dispatch(
		static_cast<UINT>(ceil(static_cast<float>(gSimulationInfo.width  >> 5) / _groupX)),
		static_cast<UINT>(ceil(static_cast<float>(gSimulationInfo.height >> 5) / _groupY)), 
		static_cast<UINT>(ceil(static_cast<float>(gSimulationInfo.depth  >> 5) / _groupZ)) 
	);
	SetBarrier(context);

	context.CSSetShaderResources(0u, 1u, _srv[A64h_rhs].GetAddressOf());
	context.CSSetUnorderedAccessViews(0u, 1u, _uav[D32h].GetAddressOf(), nullptr);
	context.CSSetShader(_interpolateCS.Get(), nullptr, 0u);
	context.Dispatch(
		static_cast<UINT>(ceil(static_cast<float>(gSimulationInfo.width  >> 5) / _groupX)),
		static_cast<UINT>(ceil(static_cast<float>(gSimulationInfo.height >> 5) / _groupY)), 
		static_cast<UINT>(ceil(static_cast<float>(gSimulationInfo.depth  >> 5) / _groupZ)) 
	);
	SetBarrier(context);

	// Jacobi for A32h Again
	{
		context.CSSetShaderResources(0u, 1u, _srv[A32h_temp].GetAddressOf());
		context.CSSetShaderResources(1u, 1u, _srv[D32h].GetAddressOf());
		context.CSSetUnorderedAccessViews(0u, 1u, _uav[A32h].GetAddressOf(), nullptr);
		context.CSSetShader(_mgJacobiCS.Get(), nullptr, 0u);
		context.Dispatch(
			static_cast<UINT>(ceil(static_cast<float>(gSimulationInfo.width  >> 5) / _groupX)),
			static_cast<UINT>(ceil(static_cast<float>(gSimulationInfo.height >> 5) / _groupY)), 
			static_cast<UINT>(ceil(static_cast<float>(gSimulationInfo.depth  >> 5) / _groupZ)) 
		);
		SetBarrier(context);

		context.CSSetShaderResources(0u, 1u, _srv[A32h].GetAddressOf());
		context.CSSetShaderResources(1u, 1u, _srv[D32h].GetAddressOf());
		context.CSSetUnorderedAccessViews(0u, 1u, _uav[A32h_temp].GetAddressOf(), nullptr);
		context.CSSetShader(_mgJacobiCS.Get(), nullptr, 0u);
		context.Dispatch(
			static_cast<UINT>(ceil(static_cast<float>(gSimulationInfo.width  >> 5) / _groupX)),
			static_cast<UINT>(ceil(static_cast<float>(gSimulationInfo.height >> 5) / _groupY)), 
			static_cast<UINT>(ceil(static_cast<float>(gSimulationInfo.depth  >> 5) / _groupZ)) 
		);
		SetBarrier(context);

		context.CSSetShaderResources(0u, 1u, _srv[A32h_temp].GetAddressOf());
		context.CSSetShaderResources(1u, 1u, _srv[D32h].GetAddressOf());
		context.CSSetUnorderedAccessViews(0u, 1u, _uav[A32h].GetAddressOf(), nullptr);
		context.CSSetShader(_mgJacobiCS.Get(), nullptr, 0u);
		context.Dispatch(
			static_cast<UINT>(ceil(static_cast<float>(gSimulationInfo.width  >> 5) / _groupX)),
			static_cast<UINT>(ceil(static_cast<float>(gSimulationInfo.height >> 5) / _groupY)), 
			static_cast<UINT>(ceil(static_cast<float>(gSimulationInfo.depth  >> 5) / _groupZ)) 
		);
		SetBarrier(context);
	}

	// A32h residual
	context.CSSetShaderResources(0u, 1u, _srv[A32h].GetAddressOf());
	context.CSSetShaderResources(1u, 1u, _srv[D32h].GetAddressOf());
	context.CSSetUnorderedAccessViews(0u, 1u, _uav[A32h_residual].GetAddressOf(), nullptr);
	context.CSSetShader(_residualCS.Get(), nullptr, 0u);
	context.Dispatch(
		static_cast<UINT>(ceil(static_cast<float>(gSimulationInfo.width  >> 5) / _groupX)),
		static_cast<UINT>(ceil(static_cast<float>(gSimulationInfo.height >> 5) / _groupY)), 
		static_cast<UINT>(ceil(static_cast<float>(gSimulationInfo.depth  >> 5) / _groupZ)) 
	);
	SetBarrier(context);

	// Interpolate A32h to A16h
	context.CSSetShaderResources(0u, 1u, _srv[A32h].GetAddressOf());
	context.CSSetUnorderedAccessViews(0u, 1u, _uav[A16h_temp].GetAddressOf(), nullptr);
	context.CSSetShader(_interpolateCS.Get(), nullptr, 0u);
	context.Dispatch(
		static_cast<UINT>(ceil(static_cast<float>(gSimulationInfo.width  >> 4) / _groupX)),
		static_cast<UINT>(ceil(static_cast<float>(gSimulationInfo.height >> 4) / _groupY)), 
		static_cast<UINT>(ceil(static_cast<float>(gSimulationInfo.depth  >> 4) / _groupZ)) 
	);
	SetBarrier(context);

	context.CSSetShaderResources(0u, 1u, _srv[A32h_rhs].GetAddressOf());
	context.CSSetUnorderedAccessViews(0u, 1u, _uav[D16h].GetAddressOf(), nullptr);
	context.CSSetShader(_interpolateCS.Get(), nullptr, 0u);
	context.Dispatch(
		static_cast<UINT>(ceil(static_cast<float>(gSimulationInfo.width  >> 4) / _groupX)),
		static_cast<UINT>(ceil(static_cast<float>(gSimulationInfo.height >> 4) / _groupY)), 
		static_cast<UINT>(ceil(static_cast<float>(gSimulationInfo.depth  >> 4) / _groupZ)) 
	);
	SetBarrier(context);

	// Jacobi for A16h Again
	{
		context.CSSetShaderResources(0u, 1u, _srv[A16h_temp].GetAddressOf());
		context.CSSetShaderResources(1u, 1u, _srv[D16h].GetAddressOf());
		context.CSSetUnorderedAccessViews(0u, 1u, _uav[A16h].GetAddressOf(), nullptr);
		context.CSSetShader(_mgJacobiCS.Get(), nullptr, 0u);
		context.Dispatch(
			static_cast<UINT>(ceil(static_cast<float>(gSimulationInfo.width  >> 4) / _groupX)),
			static_cast<UINT>(ceil(static_cast<float>(gSimulationInfo.height >> 4) / _groupY)), 
			static_cast<UINT>(ceil(static_cast<float>(gSimulationInfo.depth  >> 4) / _groupZ)) 
		);
		SetBarrier(context);

		context.CSSetShaderResources(0u, 1u, _srv[A16h].GetAddressOf());
		context.CSSetShaderResources(1u, 1u, _srv[D16h].GetAddressOf());
		context.CSSetUnorderedAccessViews(0u, 1u, _uav[A16h_temp].GetAddressOf(), nullptr);
		context.CSSetShader(_mgJacobiCS.Get(), nullptr, 0u);
		context.Dispatch(
			static_cast<UINT>(ceil(static_cast<float>(gSimulationInfo.width  >> 4) / _groupX)),
			static_cast<UINT>(ceil(static_cast<float>(gSimulationInfo.height >> 4) / _groupY)), 
			static_cast<UINT>(ceil(static_cast<float>(gSimulationInfo.depth  >> 4) / _groupZ)) 
		);
		SetBarrier(context);

		context.CSSetShaderResources(0u, 1u, _srv[A16h_temp].GetAddressOf());
		context.CSSetShaderResources(1u, 1u, _srv[D16h].GetAddressOf());
		context.CSSetUnorderedAccessViews(0u, 1u, _uav[A16h].GetAddressOf(), nullptr);
		context.CSSetShader(_mgJacobiCS.Get(), nullptr, 0u);
		context.Dispatch(
			static_cast<UINT>(ceil(static_cast<float>(gSimulationInfo.width  >> 4) / _groupX)),
			static_cast<UINT>(ceil(static_cast<float>(gSimulationInfo.height >> 4) / _groupY)), 
			static_cast<UINT>(ceil(static_cast<float>(gSimulationInfo.depth  >> 4) / _groupZ)) 
		);
		SetBarrier(context);
	}

	// A16h residual
	context.CSSetShaderResources(0u, 1u, _srv[A16h].GetAddressOf());
	context.CSSetShaderResources(1u, 1u, _srv[D16h].GetAddressOf());
	context.CSSetUnorderedAccessViews(0u, 1u, _uav[A16h_residual].GetAddressOf(), nullptr);
	context.CSSetShader(_residualCS.Get(), nullptr, 0u);
	context.Dispatch(
		static_cast<UINT>(ceil(static_cast<float>(gSimulationInfo.width  >> 4) / _groupX)),
		static_cast<UINT>(ceil(static_cast<float>(gSimulationInfo.height >> 4) / _groupY)), 
		static_cast<UINT>(ceil(static_cast<float>(gSimulationInfo.depth  >> 4) / _groupZ)) 
	);
	SetBarrier(context);

	// Interpolate A16h to A8h
	context.CSSetShaderResources(0u, 1u, _srv[A16h].GetAddressOf());
	context.CSSetUnorderedAccessViews(0u, 1u, _uav[A8h_temp].GetAddressOf(), nullptr);
	context.CSSetShader(_interpolateCS.Get(), nullptr, 0u);
	context.Dispatch(
		static_cast<UINT>(ceil(static_cast<float>(gSimulationInfo.width  >> 3) / _groupX)),
		static_cast<UINT>(ceil(static_cast<float>(gSimulationInfo.height >> 3) / _groupY)), 
		static_cast<UINT>(ceil(static_cast<float>(gSimulationInfo.depth  >> 3) / _groupZ)) 
	);
	SetBarrier(context);

	context.CSSetShaderResources(0u, 1u, _srv[A16h_rhs].GetAddressOf());
	context.CSSetUnorderedAccessViews(0u, 1u, _uav[D8h].GetAddressOf(), nullptr);
	context.CSSetShader(_interpolateCS.Get(), nullptr, 0u);
	context.Dispatch(
		static_cast<UINT>(ceil(static_cast<float>(gSimulationInfo.width  >> 3) / _groupX)),
		static_cast<UINT>(ceil(static_cast<float>(gSimulationInfo.height >> 3) / _groupY)), 
		static_cast<UINT>(ceil(static_cast<float>(gSimulationInfo.depth  >> 3) / _groupZ)) 
	);
	SetBarrier(context);

	// Jacobi for A8h Again
	{
		context.CSSetShaderResources(0u, 1u, _srv[A8h_temp].GetAddressOf());
		context.CSSetShaderResources(1u, 1u, _srv[D8h].GetAddressOf());
		context.CSSetUnorderedAccessViews(0u, 1u, _uav[A8h].GetAddressOf(), nullptr);
		context.CSSetShader(_mgJacobiCS.Get(), nullptr, 0u);
		context.Dispatch(
			static_cast<UINT>(ceil(static_cast<float>(gSimulationInfo.width  >> 3) / _groupX)),
			static_cast<UINT>(ceil(static_cast<float>(gSimulationInfo.height >> 3) / _groupY)), 
			static_cast<UINT>(ceil(static_cast<float>(gSimulationInfo.depth  >> 3) / _groupZ)) 
		);
		SetBarrier(context);

		context.CSSetShaderResources(0u, 1u, _srv[A8h].GetAddressOf());
		context.CSSetShaderResources(1u, 1u, _srv[D8h].GetAddressOf());
		context.CSSetUnorderedAccessViews(0u, 1u, _uav[A8h_temp].GetAddressOf(), nullptr);
		context.CSSetShader(_mgJacobiCS.Get(), nullptr, 0u);
		context.Dispatch(
			static_cast<UINT>(ceil(static_cast<float>(gSimulationInfo.width  >> 3) / _groupX)),
			static_cast<UINT>(ceil(static_cast<float>(gSimulationInfo.height >> 3) / _groupY)), 
			static_cast<UINT>(ceil(static_cast<float>(gSimulationInfo.depth  >> 3) / _groupZ)) 
		);
		SetBarrier(context);

		context.CSSetShaderResources(0u, 1u, _srv[A8h_temp].GetAddressOf());
		context.CSSetShaderResources(1u, 1u, _srv[D8h].GetAddressOf());
		context.CSSetUnorderedAccessViews(0u, 1u, _uav[A8h].GetAddressOf(), nullptr);
		context.CSSetShader(_mgJacobiCS.Get(), nullptr, 0u);
		context.Dispatch(
			static_cast<UINT>(ceil(static_cast<float>(gSimulationInfo.width  >> 3) / _groupX)),
			static_cast<UINT>(ceil(static_cast<float>(gSimulationInfo.height >> 3) / _groupY)), 
			static_cast<UINT>(ceil(static_cast<float>(gSimulationInfo.depth  >> 3) / _groupZ)) 
		);
		SetBarrier(context);
	}

	// A8h residual
	context.CSSetShaderResources(0u, 1u, _srv[A8h].GetAddressOf());
	context.CSSetShaderResources(1u, 1u, _srv[D8h].GetAddressOf());
	context.CSSetUnorderedAccessViews(0u, 1u, _uav[A8h_residual].GetAddressOf(), nullptr);
	context.CSSetShader(_residualCS.Get(), nullptr, 0u);
	context.Dispatch(
		static_cast<UINT>(ceil(static_cast<float>(gSimulationInfo.width  >> 3) / _groupX)),
		static_cast<UINT>(ceil(static_cast<float>(gSimulationInfo.height >> 3) / _groupY)), 
		static_cast<UINT>(ceil(static_cast<float>(gSimulationInfo.depth  >> 3) / _groupZ)) 
	);
	SetBarrier(context);

	// Interpolate A8h to A4h
	context.CSSetShaderResources(0u, 1u, _srv[A8h].GetAddressOf());
	context.CSSetUnorderedAccessViews(0u, 1u, _uav[A4h_temp].GetAddressOf(), nullptr);
	context.CSSetShader(_interpolateCS.Get(), nullptr, 0u);
	context.Dispatch(
		static_cast<UINT>(ceil(static_cast<float>(gSimulationInfo.width  >> 2) / _groupX)),
		static_cast<UINT>(ceil(static_cast<float>(gSimulationInfo.height >> 2) / _groupY)), 
		static_cast<UINT>(ceil(static_cast<float>(gSimulationInfo.depth  >> 2) / _groupZ)) 
	);
	SetBarrier(context);

	context.CSSetShaderResources(0u, 1u, _srv[A8h_rhs].GetAddressOf());
	context.CSSetUnorderedAccessViews(0u, 1u, _uav[D4h].GetAddressOf(), nullptr);
	context.CSSetShader(_interpolateCS.Get(), nullptr, 0u);
	context.Dispatch(
		static_cast<UINT>(ceil(static_cast<float>(gSimulationInfo.width  >> 2) / _groupX)),
		static_cast<UINT>(ceil(static_cast<float>(gSimulationInfo.height >> 2) / _groupY)), 
		static_cast<UINT>(ceil(static_cast<float>(gSimulationInfo.depth  >> 2) / _groupZ)) 
	);
	SetBarrier(context);

	// Jacobi for A4h Again
	{
		context.CSSetShaderResources(0u, 1u, _srv[A4h_temp].GetAddressOf());
		context.CSSetShaderResources(1u, 1u, _srv[D4h].GetAddressOf());
		context.CSSetUnorderedAccessViews(0u, 1u, _uav[A4h].GetAddressOf(), nullptr);
		context.CSSetShader(_mgJacobiCS.Get(), nullptr, 0u);
		context.Dispatch(
			static_cast<UINT>(ceil(static_cast<float>(gSimulationInfo.width  >> 2) / _groupX)),
			static_cast<UINT>(ceil(static_cast<float>(gSimulationInfo.height >> 2) / _groupY)), 
			static_cast<UINT>(ceil(static_cast<float>(gSimulationInfo.depth  >> 2) / _groupZ)) 
		);
		SetBarrier(context);

		context.CSSetShaderResources(0u, 1u, _srv[A4h].GetAddressOf());
		context.CSSetShaderResources(1u, 1u, _srv[D4h].GetAddressOf());
		context.CSSetUnorderedAccessViews(0u, 1u, _uav[A4h_temp].GetAddressOf(), nullptr);
		context.CSSetShader(_mgJacobiCS.Get(), nullptr, 0u);
		context.Dispatch(
			static_cast<UINT>(ceil(static_cast<float>(gSimulationInfo.width  >> 2) / _groupX)),
			static_cast<UINT>(ceil(static_cast<float>(gSimulationInfo.height >> 2) / _groupY)), 
			static_cast<UINT>(ceil(static_cast<float>(gSimulationInfo.depth  >> 2) / _groupZ)) 
		);
		SetBarrier(context);

		context.CSSetShaderResources(0u, 1u, _srv[A4h_temp].GetAddressOf());
		context.CSSetShaderResources(1u, 1u, _srv[D4h].GetAddressOf());
		context.CSSetUnorderedAccessViews(0u, 1u, _uav[A4h].GetAddressOf(), nullptr);
		context.CSSetShader(_mgJacobiCS.Get(), nullptr, 0u);
		context.Dispatch(
			static_cast<UINT>(ceil(static_cast<float>(gSimulationInfo.width  >> 2) / _groupX)),
			static_cast<UINT>(ceil(static_cast<float>(gSimulationInfo.height >> 2) / _groupY)), 
			static_cast<UINT>(ceil(static_cast<float>(gSimulationInfo.depth  >> 2) / _groupZ)) 
		);
		SetBarrier(context);
	}

	// A4h residual
	context.CSSetShaderResources(0u, 1u, _srv[A4h].GetAddressOf());
	context.CSSetShaderResources(1u, 1u, _srv[D4h].GetAddressOf());
	context.CSSetUnorderedAccessViews(0u, 1u, _uav[A4h_residual].GetAddressOf(), nullptr);
	context.CSSetShader(_residualCS.Get(), nullptr, 0u);
	context.Dispatch(
		static_cast<UINT>(ceil(static_cast<float>(gSimulationInfo.width  >> 2) / _groupX)),
		static_cast<UINT>(ceil(static_cast<float>(gSimulationInfo.height >> 2) / _groupY)), 
		static_cast<UINT>(ceil(static_cast<float>(gSimulationInfo.depth  >> 2) / _groupZ)) 
	);
	SetBarrier(context);

	// Interpolate A4h to A2h
	context.CSSetShaderResources(0u, 1u, _srv[A4h].GetAddressOf());
	context.CSSetUnorderedAccessViews(0u, 1u, _uav[A2h_temp].GetAddressOf(), nullptr);
	context.CSSetShader(_interpolateCS.Get(), nullptr, 0u);
	context.Dispatch(
		static_cast<UINT>(ceil(static_cast<float>(gSimulationInfo.width  >> 1) / _groupX)),
		static_cast<UINT>(ceil(static_cast<float>(gSimulationInfo.height >> 1) / _groupY)), 
		static_cast<UINT>(ceil(static_cast<float>(gSimulationInfo.depth  >> 1) / _groupZ)) 
	);
	SetBarrier(context);

	context.CSSetShaderResources(0u, 1u, _srv[A4h_rhs].GetAddressOf());
	context.CSSetUnorderedAccessViews(0u, 1u, _uav[D2h].GetAddressOf(), nullptr);
	context.CSSetShader(_interpolateCS.Get(), nullptr, 0u);
	context.Dispatch(
		static_cast<UINT>(ceil(static_cast<float>(gSimulationInfo.width  >> 1) / _groupX)),
		static_cast<UINT>(ceil(static_cast<float>(gSimulationInfo.height >> 1) / _groupY)), 
		static_cast<UINT>(ceil(static_cast<float>(gSimulationInfo.depth  >> 1) / _groupZ)) 
	);
	SetBarrier(context);

	// Jacobi for A2h
	{
		context.CSSetShaderResources(0u, 1u, _srv[A2h_temp].GetAddressOf());
		context.CSSetShaderResources(1u, 1u, _srv[D2h].GetAddressOf());
		context.CSSetUnorderedAccessViews(0u, 1u, _uav[A2h].GetAddressOf(), nullptr);
		context.CSSetShader(_mgJacobiCS.Get(), nullptr, 0u);
		context.Dispatch(
			static_cast<UINT>(ceil(static_cast<float>(gSimulationInfo.width  >> 1) / _groupX)),
			static_cast<UINT>(ceil(static_cast<float>(gSimulationInfo.height >> 1) / _groupY)), 
			static_cast<UINT>(ceil(static_cast<float>(gSimulationInfo.depth  >> 1) / _groupZ)) 
		);
		SetBarrier(context);

		context.CSSetShaderResources(0u, 1u, _srv[A2h].GetAddressOf());
		context.CSSetShaderResources(1u, 1u, _srv[D2h].GetAddressOf());
		context.CSSetUnorderedAccessViews(0u, 1u, _uav[A2h_temp].GetAddressOf(), nullptr);
		context.CSSetShader(_mgJacobiCS.Get(), nullptr, 0u);
		context.Dispatch(
			static_cast<UINT>(ceil(static_cast<float>(gSimulationInfo.width  >> 1) / _groupX)),
			static_cast<UINT>(ceil(static_cast<float>(gSimulationInfo.height >> 1) / _groupY)), 
			static_cast<UINT>(ceil(static_cast<float>(gSimulationInfo.depth  >> 1) / _groupZ)) 
		);
		SetBarrier(context);

		context.CSSetShaderResources(0u, 1u, _srv[A2h_temp].GetAddressOf());
		context.CSSetShaderResources(1u, 1u, _srv[D2h].GetAddressOf());
		context.CSSetUnorderedAccessViews(0u, 1u, _uav[A2h].GetAddressOf(), nullptr);
		context.CSSetShader(_mgJacobiCS.Get(), nullptr, 0u);
		context.Dispatch(
			static_cast<UINT>(ceil(static_cast<float>(gSimulationInfo.width  >> 1) / _groupX)),
			static_cast<UINT>(ceil(static_cast<float>(gSimulationInfo.height >> 1) / _groupY)), 
			static_cast<UINT>(ceil(static_cast<float>(gSimulationInfo.depth  >> 1) / _groupZ)) 
		);
		SetBarrier(context);
	}

	// A2h residual
	context.CSSetShaderResources(0u, 1u, _srv[A2h].GetAddressOf());
	context.CSSetShaderResources(1u, 1u, _srv[D2h].GetAddressOf());
	context.CSSetUnorderedAccessViews(0u, 1u, _uav[A2h_residual].GetAddressOf(), nullptr);
	context.CSSetShader(_residualCS.Get(), nullptr, 0u);
	context.Dispatch(
		static_cast<UINT>(ceil(static_cast<float>(gSimulationInfo.width  >> 1) / _groupX)),
		static_cast<UINT>(ceil(static_cast<float>(gSimulationInfo.height >> 1) / _groupY)), 
		static_cast<UINT>(ceil(static_cast<float>(gSimulationInfo.depth  >> 1) / _groupZ)) 
	);
	SetBarrier(context);

	// Interpolate A2h to Ah
	context.CSSetShaderResources(0u, 1u, _srv[A2h].GetAddressOf());
	context.CSSetUnorderedAccessViews(0u, 1u, _uav[Ah_temp].GetAddressOf(), nullptr);
	context.CSSetShader(_interpolateCS.Get(), nullptr, 0u);
	context.Dispatch(
		static_cast<UINT>(ceil(static_cast<float>(gSimulationInfo.width) / _groupX)),
		static_cast<UINT>(ceil(static_cast<float>(gSimulationInfo.height) / _groupY)), 
		static_cast<UINT>(ceil(static_cast<float>(gSimulationInfo.depth) / _groupZ)) 
	);
	SetBarrier(context);

	context.CSSetShaderResources(0u, 1u, _srv[A2h_rhs].GetAddressOf());
	context.CSSetUnorderedAccessViews(0u, 1u, _uav[Dh].GetAddressOf(), nullptr);
	context.CSSetShader(_interpolateCS.Get(), nullptr, 0u);
	context.Dispatch(
		static_cast<UINT>(ceil(static_cast<float>(gSimulationInfo.width) / _groupX)),
		static_cast<UINT>(ceil(static_cast<float>(gSimulationInfo.height) / _groupY)), 
		static_cast<UINT>(ceil(static_cast<float>(gSimulationInfo.depth) / _groupZ)) 
	);
	SetBarrier(context);

	// Jacobi for Ah Again
	{
		ID3D11ShaderResourceView* srv[3]{ _srv[Ah_temp].Get(), _srv[Dh].Get() };
		context.CSSetShaderResources(0u, 2u, srv);
		context.CSSetUnorderedAccessViews(0u, 1u, _uav[Final].GetAddressOf(), nullptr);
		context.CSSetShader(_cs.Get(), nullptr, 0u);
		context.Dispatch(
			static_cast<UINT>(ceil(static_cast<float>(gSimulationInfo.width) / _groupX)),
			static_cast<UINT>(ceil(static_cast<float>(gSimulationInfo.height) / _groupY)), 
			static_cast<UINT>(ceil(static_cast<float>(gSimulationInfo.depth) / _groupZ)) 
		);
		SetBarrier(context);

		srv[0] = _srv[Final].Get();
		context.CSSetShaderResources(0u, 2u, srv);
		context.CSSetUnorderedAccessViews(0u, 1u, _uav[Ah_temp].GetAddressOf(), nullptr);
		context.CSSetShader(_cs.Get(), nullptr, 0u);
		context.Dispatch(
			static_cast<UINT>(ceil(static_cast<float>(gSimulationInfo.width) / _groupX)),
			static_cast<UINT>(ceil(static_cast<float>(gSimulationInfo.height) / _groupY)), 
			static_cast<UINT>(ceil(static_cast<float>(gSimulationInfo.depth) / _groupZ)) 
		);
		SetBarrier(context);

		srv[0] = _srv[Ah_temp].Get();
		context.CSSetShaderResources(0u, 2u, srv);
		context.CSSetUnorderedAccessViews(0u, 1u, _uav[Final].GetAddressOf(), nullptr);
		context.CSSetShader(_cs.Get(), nullptr, 0u);
		context.Dispatch(
			static_cast<UINT>(ceil(static_cast<float>(gSimulationInfo.width) / _groupX)),
			static_cast<UINT>(ceil(static_cast<float>(gSimulationInfo.height) / _groupY)), 
			static_cast<UINT>(ceil(static_cast<float>(gSimulationInfo.depth) / _groupZ)) 
		);
		SetBarrier(context);
	}
}

void MultiGrid3D1DStage::Consume(ID3D11Resource* resource, int32_t attribute_id)
{
    if (attribute_id == _divID)
        pDevice->CreateShaderResourceView(resource, nullptr, _srv[Dh].ReleaseAndGetAddressOf());
}

ID3D11Resource* MultiGrid3D1DStage::Expose(int32_t attribute_id)
{
    if (attribute_id == _pressureOutID)
        return static_cast<ID3D11Resource*>(_resource[Final].Get());
	return nullptr;
}

