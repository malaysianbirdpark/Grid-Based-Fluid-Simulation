#include "pch.h"
#include "VoxelizationStage.h"

#include "DrawSceneStage.h"
#include "DrawVolumeStage.h"
#include "Transform.h"
#include "PipelineStateObject.h"

#include "CBVoxelization.h"

#include <d3dcompiler.h>

VoxelizationStage::VoxelizationStage(ID3D11DeviceContext& context)
{
    _vp.Width = static_cast<float>(gSimulationInfo.width);
    _vp.Height = static_cast<float>(gSimulationInfo.height);
    _vp.MinDepth = 0.0f;
    _vp.MaxDepth = 1.0f;
    _vp.TopLeftX = 0.0f;
    _vp.TopLeftY = 0.0f;

    // voxelization
    {
		{
			D3D11_DEPTH_STENCIL_DESC desc{ CD3D11_DEPTH_STENCIL_DESC{CD3D11_DEFAULT{}} };

            desc.DepthEnable = TRUE;
            desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
            desc.DepthFunc = D3D11_COMPARISON_LESS;

            desc.StencilEnable = TRUE;
            desc.StencilReadMask = D3D11_DEFAULT_STENCIL_READ_MASK;
            desc.StencilWriteMask = D3D11_DEFAULT_STENCIL_WRITE_MASK;

            desc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_ZERO;
            desc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_ZERO;
            desc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_DECR;
            desc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

            desc.BackFace.StencilFailOp = D3D11_STENCIL_OP_ZERO;
            desc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_ZERO;
            desc.BackFace.StencilPassOp = D3D11_STENCIL_OP_INCR;
            desc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

			pDevice->CreateDepthStencilState(&desc, _voxelDS.ReleaseAndGetAddressOf());
		}

		{
			D3D11_DEPTH_STENCIL_DESC desc{ CD3D11_DEPTH_STENCIL_DESC{CD3D11_DEFAULT{}} };

            desc.DepthEnable = TRUE;
            desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
            desc.DepthFunc = D3D11_COMPARISON_LESS;

            desc.StencilEnable = TRUE;
            desc.StencilReadMask = D3D11_DEFAULT_STENCIL_READ_MASK;
            desc.StencilWriteMask = 0u;

            desc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
            desc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
            desc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
            desc.FrontFace.StencilFunc = D3D11_COMPARISON_NOT_EQUAL;

            desc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
            desc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
            desc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
            desc.BackFace.StencilFunc = D3D11_COMPARISON_NOT_EQUAL;

			pDevice->CreateDepthStencilState(&desc, _readDS.ReleaseAndGetAddressOf());
		}

		D3D11_TEXTURE2D_DESC descDepth{};
		descDepth.Usage = D3D11_USAGE_DEFAULT;
		descDepth.BindFlags = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;
		descDepth.Width = gSimulationInfo.width;
		descDepth.Height = gSimulationInfo.height;
		descDepth.MipLevels = 1u;
		descDepth.ArraySize = gSimulationInfo.depth;
		descDepth.CPUAccessFlags = 0u;
		descDepth.SampleDesc.Count = 1u;
		descDepth.SampleDesc.Quality = 0u;

		descDepth.Format = DXGI_FORMAT_R24G8_TYPELESS;
		pDevice->CreateTexture2D(&descDepth, nullptr, _voxelDSB.ReleaseAndGetAddressOf());

		_voxelDSV.resize(gSimulationInfo.depth);

		D3D11_DEPTH_STENCIL_VIEW_DESC desc_dsv{};
		desc_dsv.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
		desc_dsv.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2DARRAY;
		desc_dsv.Texture2DArray.MipSlice = 0u;
		desc_dsv.Texture2DArray.ArraySize = 1u;
		for (uint32_t i{ 0 }; i != _voxelDSV.size(); ++i) {
			desc_dsv.Texture2DArray.FirstArraySlice = static_cast<UINT>(i);
			pDevice->CreateDepthStencilView(_voxelDSB.Get(), &desc_dsv, _voxelDSV[i].ReleaseAndGetAddressOf());
		}

		D3D11_SHADER_RESOURCE_VIEW_DESC srv_desc{ CD3D11_SHADER_RESOURCE_VIEW_DESC{} };
		srv_desc.Format = DXGI_FORMAT_X24_TYPELESS_G8_UINT;
		srv_desc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2DARRAY;
		srv_desc.Texture2DArray.MipLevels = 1u;
		srv_desc.Texture2DArray.ArraySize = gSimulationInfo.depth;
		srv_desc.Texture2DArray.FirstArraySlice = 0u;
		srv_desc.Texture2DArray.MostDetailedMip = 0u;
		pDevice->CreateShaderResourceView(_voxelDSB.Get(), &srv_desc, _dsvSRV.ReleaseAndGetAddressOf());
    }

	{
		Microsoft::WRL::ComPtr<ID3D11Texture2D> _voxelTex;
		D3D11_TEXTURE2D_DESC desc{};
		desc.Usage = D3D11_USAGE_DEFAULT;
		desc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_UNORDERED_ACCESS;
		desc.Width = gSimulationInfo.width;
		desc.Height = gSimulationInfo.height;
		desc.MipLevels = 1u;
		desc.ArraySize = gSimulationInfo.depth;
		desc.CPUAccessFlags = 0u;
		desc.SampleDesc.Count = 1u;
		desc.SampleDesc.Quality = 0u;

		desc.Format = DXGI_FORMAT_R8_UNORM;
		pDevice->CreateTexture2D(&desc, nullptr, _voxelTex.ReleaseAndGetAddressOf());

		pDevice->CreateShaderResourceView(_voxelTex.Get(), nullptr, _voxelSRV.ReleaseAndGetAddressOf());
		pDevice->CreateUnorderedAccessView(_voxelTex.Get(), nullptr, _voxelUAV.ReleaseAndGetAddressOf());
	}

	{
		std::vector<D3D11_INPUT_ELEMENT_DESC> input_elem_desc{
			{"POSITION", 0u, DXGI_FORMAT_R32G32B32A32_FLOAT, 0u, 0u, D3D11_INPUT_PER_VERTEX_DATA, 0u},
			{"POSITION", 1u, DXGI_FORMAT_R32G32B32A32_FLOAT, 1u, 0u, D3D11_INPUT_PER_VERTEX_DATA, 0u},
		};

		_obstaclePSO = std::make_unique<PipelineStateObject>();
		_obstaclePSO->SetVertexShader("./CSO/Voxelization_VS.cso");
		_obstaclePSO->SetInputLayout(input_elem_desc);
		//_obstaclePSO->SetGeometryShader("./CSO/Voxelization_GS.cso");
		_obstaclePSO->SetPixelShader("./CSO/Voxelization_PS.cso");
	}

    // no culling
    {
		D3D11_RASTERIZER_DESC rd{ CD3D11_RASTERIZER_DESC{} };

        rd.FillMode = D3D11_FILL_MODE::D3D11_FILL_SOLID;
        rd.CullMode = D3D11_CULL_MODE::D3D11_CULL_NONE;
        rd.FrontCounterClockwise = false;
        rd.DepthClipEnable = true;
        rd.MultisampleEnable = false;

        pDevice->CreateRasterizerState(&rd, _rs.ReleaseAndGetAddressOf());
    }

	_inverseTransform = std::make_unique<InverseTransform>(context);

	// bounding box compute shader
    {
        std::string _path{"./CSO/BoundingBox3D_CS.cso"};
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
            _boundingBoxCS.ReleaseAndGetAddressOf()
        );
    }

	// Velocity Calculation
	{
		Microsoft::WRL::ComPtr<ID3D11Texture2D> _velocityTex;
		D3D11_TEXTURE2D_DESC desc{};
		desc.Usage = D3D11_USAGE_DEFAULT;
		desc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_UNORDERED_ACCESS;
		desc.Width = gSimulationInfo.width;
		desc.Height = gSimulationInfo.height;
		desc.MipLevels = 1u;
		desc.ArraySize = gSimulationInfo.depth;
		desc.CPUAccessFlags = 0u;
		desc.SampleDesc.Count = 1u;
		desc.SampleDesc.Quality = 0u;

		desc.Format = DXGI_FORMAT_R16G16B16A16_FLOAT;
		pDevice->CreateTexture2D(&desc, nullptr, _velocityTex.ReleaseAndGetAddressOf());

		_velocityRTV.resize(gSimulationInfo.depth);

		D3D11_RENDER_TARGET_VIEW_DESC rtv_desc{};
		rtv_desc.Format = DXGI_FORMAT_R16G16B16A16_FLOAT;
		rtv_desc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2DARRAY;
		rtv_desc.Texture2DArray.MipSlice = 0u;
		rtv_desc.Texture2DArray.ArraySize = 1u;
		for (uint32_t i{ 0 }; i != _velocityRTV.size(); ++i) {
			rtv_desc.Texture2DArray.FirstArraySlice = static_cast<UINT>(i);
			pDevice->CreateRenderTargetView(_velocityTex.Get(), &rtv_desc, _velocityRTV[i].ReleaseAndGetAddressOf());
		}

		pDevice->CreateShaderResourceView(_velocityTex.Get(), nullptr, _velocitySRV.ReleaseAndGetAddressOf());
		pDevice->CreateUnorderedAccessView(_velocityTex.Get(), nullptr, _velocityUAV.ReleaseAndGetAddressOf());
	}

	{
		std::vector<D3D11_INPUT_ELEMENT_DESC> input_elem_desc{
			{"POSITION", 0u, DXGI_FORMAT_R32G32B32A32_FLOAT, 0u, 0u, D3D11_INPUT_PER_VERTEX_DATA, 0u},
			{"POSITION", 1u, DXGI_FORMAT_R32G32B32A32_FLOAT, 1u, 0u, D3D11_INPUT_PER_VERTEX_DATA, 0u},
		};

		_vertexVelocityPSO = std::make_unique<PipelineStateObject>();
		_vertexVelocityPSO->SetVertexShader("./CSO/VertexVelocity_VS.cso");
		_vertexVelocityPSO->SetInputLayout(input_elem_desc);
		//_vertexVelocityPSO->SetGeometryShader("./CSO/VertexVelocity_GS.cso");
		_vertexVelocityPSO->SetPixelShader("./CSO/VertexVelocity_PS.cso");
	}

	_cb = std::make_unique<CBVoxelization>();
}

void VoxelizationStage::Run(ID3D11DeviceContext& context)
{
	static float constexpr clear_color[4]{ 0.0f, 0.0f, 0.0f, 0.0f };

	context.RSSetViewports(1u, &_vp);
	context.RSSetState(_rs.Get());

	_inverseTransform->SetModel(DirectX::XMMatrixIdentity());

	for (auto depth{ 0 }; depth != gSimulationInfo.depth; ++depth) {
		context.ClearRenderTargetView(_velocityRTV[depth].Get(), clear_color);
		context.ClearDepthStencilView(_voxelDSV[depth].Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0u);
	}

	// Obstacle
	_obstaclePSO->Bind(context);
    context.OMSetDepthStencilState(_voxelDS.Get(), 1u);
	for (auto depth{ 0 }; depth != gSimulationInfo.depth; ++depth) {
		_inverseTransform->SetProj(
            DirectX::XMMatrixOrthographicOffCenterLH(
                -0.5f, 0.5f, -0.5f, 0.5f,
				static_cast<float>(depth) / gSimulationInfo.depth,
                20000.0f
            )
		);
		_inverseTransform->Update(context);
		_inverseTransform->Bind(context);

		// draw target object
		context.OMSetRenderTargets(1u, _velocityRTV[depth].GetAddressOf(), _voxelDSV[depth].Get());
		_targetScene->RawDraw(context);
	}

	context.OMSetRenderTargets(0u, nullptr, nullptr);
    context.CSSetShaderResources(0u, 1u, _dsvSRV.GetAddressOf());
    context.CSSetUnorderedAccessViews(0u, 1u, _voxelUAV.GetAddressOf(), nullptr);
    context.CSSetShader(_boundingBoxCS.Get(), nullptr, 0u);
    context.Dispatch(
        static_cast<UINT>(ceil(static_cast<float>(gSimulationInfo.width) / 8)),
        static_cast<UINT>(ceil(static_cast<float>(gSimulationInfo.height) / 8)),
        static_cast<UINT>(ceil(static_cast<float>(gSimulationInfo.depth) / 8))
    );

	static ID3D11ShaderResourceView* const barrier0[1]{ nullptr };
	static ID3D11UnorderedAccessView* const barrier1[1]{ nullptr };
    context.CSSetShaderResources(0u, 1u, barrier0);
    context.CSSetUnorderedAccessViews(0u, 1u, barrier1, nullptr);

	ID3D11ShaderResourceView* const voxels[2]{ _voxelSRV.Get(), _velocitySRV.Get() };
	context.CSSetShaderResources(6u, 2u, voxels);
}

void VoxelizationStage::AddTargetScene(std::shared_ptr<class DrawSceneStage> target)
{
	_targetScene = std::move(target);
}

void VoxelizationStage::AddTargetVolume(std::shared_ptr<class DrawVolumeStage> target)
{
	_targetVolume = std::move(target);
}
