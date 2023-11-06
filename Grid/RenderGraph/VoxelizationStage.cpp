#include "pch.h"
#include "VoxelizationStage.h"

#include "DrawSceneStage.h"
#include "DrawVolumeStage.h"
#include "Transform.h"
#include "PipelineStateObject.h"

VoxelizationStage::VoxelizationStage(ID3D11DeviceContext& context)
{
    // voxelization
    {
		{
			D3D11_DEPTH_STENCIL_DESC desc{ CD3D11_DEPTH_STENCIL_DESC{CD3D11_DEFAULT{}} };

            desc.DepthEnable = TRUE;
            desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
            desc.DepthFunc = D3D11_COMPARISON_ALWAYS;

            desc.StencilEnable = TRUE;
            desc.StencilReadMask = D3D11_DEFAULT_STENCIL_READ_MASK;
            desc.StencilWriteMask = D3D11_DEFAULT_STENCIL_WRITE_MASK;

            desc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_INCR;
            desc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
            desc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_INCR;
            desc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

            desc.BackFace.StencilFailOp = D3D11_STENCIL_OP_DECR;
            desc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
            desc.BackFace.StencilPassOp = D3D11_STENCIL_OP_DECR;
            desc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

			pDevice->CreateDepthStencilState(&desc, _voxelDS.ReleaseAndGetAddressOf());
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
    }

	{
		Microsoft::WRL::ComPtr<ID3D11Texture2D> _nulltex;
		D3D11_TEXTURE2D_DESC desc{};
		desc.Usage = D3D11_USAGE_DEFAULT;
		desc.BindFlags = D3D11_BIND_RENDER_TARGET;
		desc.Width = gSimulationInfo.width;
		desc.Height = gSimulationInfo.height;
		desc.MipLevels = 1u;
		desc.ArraySize = 1u;
		desc.CPUAccessFlags = 0u;
		desc.SampleDesc.Count = 1u;
		desc.SampleDesc.Quality = 0u;

		desc.Format = DXGI_FORMAT_R8_UNORM;
		pDevice->CreateTexture2D(&desc, nullptr, _nulltex.ReleaseAndGetAddressOf());
		pDevice->CreateRenderTargetView(_nulltex.Get(), nullptr, _nullRTV.ReleaseAndGetAddressOf());
	}

	{
		std::vector<D3D11_INPUT_ELEMENT_DESC> input_elem_desc{
			{"POSITION", 0u, DXGI_FORMAT_R32G32B32_FLOAT, 0u, 0u, D3D11_INPUT_PER_VERTEX_DATA, 0u},
			{"NORMAL", 0u, DXGI_FORMAT_R32G32B32_FLOAT, 0u, sizeof(DirectX::XMFLOAT3), D3D11_INPUT_PER_VERTEX_DATA, 0u},
			{"TANGENT", 0u, DXGI_FORMAT_R32G32B32_FLOAT, 0u, sizeof(DirectX::XMFLOAT3) * 2, D3D11_INPUT_PER_VERTEX_DATA, 0u},
			{"BINORMAL", 0u, DXGI_FORMAT_R32G32B32_FLOAT, 0u, sizeof(DirectX::XMFLOAT3) * 3, D3D11_INPUT_PER_VERTEX_DATA, 0u},
			{"TEXCOORD", 0u, DXGI_FORMAT_R32G32_FLOAT, 0u, sizeof(DirectX::XMFLOAT3) * 4, D3D11_INPUT_PER_VERTEX_DATA, 0u},
		};

		_pso = std::make_unique<PipelineStateObject>();
		_pso->SetVertexShader("./CSO/Voxelization_VS.cso");
		_pso->SetInputLayout(input_elem_desc);
		_pso->SetPixelShader("./CSO/Voxelization_PS.cso");
	}

    // no culling
    {
		D3D11_RASTERIZER_DESC rd{};

        rd.FillMode = D3D11_FILL_MODE::D3D11_FILL_SOLID;
        rd.CullMode = D3D11_CULL_MODE::D3D11_CULL_NONE;
        rd.FrontCounterClockwise = false;
        rd.DepthClipEnable = true;
        rd.MultisampleEnable = false;

        pDevice->CreateRasterizerState(&rd, _rs.ReleaseAndGetAddressOf());
    }

	_inverseTransform = std::make_unique<InverseTransform>(context);
}

void VoxelizationStage::Run(ID3D11DeviceContext& context)
{
	static float constexpr clear_color[4]{ 0.0f, 0.0f, 0.0f, 0.0f };
	_pso->Bind(context);

	context.RSSetState(_rs.Get());
    context.OMSetDepthStencilState(_voxelDS.Get(), 1u);

	_inverseTransform->SetModel(DirectX::XMMatrixIdentity());

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

		context.OMSetRenderTargets(1u, _nullRTV.GetAddressOf(), _voxelDSV[depth].Get());
		context.ClearRenderTargetView(_nullRTV.Get(), clear_color);
		context.ClearDepthStencilView(_voxelDSV[depth].Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0u);

		// draw target object
		_targetScene->RawDraw(context);
	}
}

void VoxelizationStage::AddTargetScene(std::shared_ptr<class DrawSceneStage> target)
{
	_targetScene = std::move(target);
}

void VoxelizationStage::AddTargetVolume(std::shared_ptr<class DrawVolumeStage> target)
{
	_targetVolume = std::move(target);
}
