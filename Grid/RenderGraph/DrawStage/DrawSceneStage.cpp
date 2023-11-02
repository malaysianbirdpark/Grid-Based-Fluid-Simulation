#include "pch.h"
#include "DrawSceneStage.h"

#include "imnodes.h"

#include "Transform.h"
#include "PipelineStateObject.h"

#include "Transform.h"

DrawSceneStage::DrawSceneStage(ID3D11DeviceContext& context, char const* name)
	: DrawStage{name}
{
    _scene.Init(context, "./Assets/DragonAttenuation/DragonAttenuation.gltf", "Dragon");
    _transform = std::make_unique<Transform>(context, DirectX::XMMatrixIdentity());

    _stack.resize(_scene._tree.size());
    _visited.resize(_scene._tree.size());

    std::vector<D3D11_INPUT_ELEMENT_DESC> input_elem_desc{
        {"POSITION", 0u, DXGI_FORMAT_R32G32B32_FLOAT, 0u, 0u, D3D11_INPUT_PER_VERTEX_DATA, 0u},
        {"NORMAL", 0u, DXGI_FORMAT_R32G32B32_FLOAT, 0u, sizeof(DirectX::XMFLOAT3), D3D11_INPUT_PER_VERTEX_DATA, 0u},
        {"TANGENT", 0u, DXGI_FORMAT_R32G32B32_FLOAT, 0u, sizeof(DirectX::XMFLOAT3) * 2, D3D11_INPUT_PER_VERTEX_DATA, 0u},
        {"BINORMAL", 0u, DXGI_FORMAT_R32G32B32_FLOAT, 0u, sizeof(DirectX::XMFLOAT3) * 3, D3D11_INPUT_PER_VERTEX_DATA, 0u},
        {"TEXCOORD", 0u, DXGI_FORMAT_R32G32_FLOAT, 0u, sizeof(DirectX::XMFLOAT3) * 4, D3D11_INPUT_PER_VERTEX_DATA, 0u},
    };

    _pso.push_back(std::move(std::make_unique<PipelineStateObject>()));
	_pso.back()->SetVertexShader("./CSO/SolidTex_VS.cso");
	_pso.back()->SetInputLayout(input_elem_desc);
	_pso.back()->SetPixelShader("./CSO/SolidTex_PS.cso");

 //   _pso.push_back(std::move(std::make_unique<PipelineStateObject>()));
	//_pso.back()->SetVertexShader("./CSO/SolidTex_VS.cso");
	//_pso.back()->SetInputLayout(input_elem_desc);
	//_pso.back()->SetPixelShader("./CSO/SolidTex_PS.cso");

    InitRS();
    InitDS();
    InitBS();
}

void DrawSceneStage::Run(ID3D11DeviceContext& context)
{
    context.RSSetViewports(1u, &_vp);
    context.RSSetState(_rs.Get());
    context.OMSetDepthStencilState(_gDS.Get(), 0u);
    context.OMSetBlendState(_bs.Get(), nullptr, 0xFFFFFFFFu);
    //context.OMSetBlendState(nullptr, nullptr, 0xFFFFFFFFu);

    context.OMSetRenderTargets(1u, _resultRTV.GetAddressOf(), _gDSV.Get());
    context.ClearRenderTargetView(_resultRTV.Get(), clear_color);

    //context.PSSetShaderResources(0u, 1u, _previous.GetAddressOf());

    _pso.front()->Bind(context);

    using namespace DirectX;

    _stack.clear();
    for (auto i{ 1 }; i != _visited.size(); ++i)
        _visited[i] = false;

    _stack.push_back(0);
    _visited[0] = true;
    while (!_stack.empty()) {
        auto const node{ _stack.back() };
        _stack.pop_back();

        if (_scene._nodeId_to_meshId.contains(node)) {
            _transform->SetModel(XMLoadFloat4x4(&_scene._globalTransforms[node]));
            _transform->Update(context);

            _transform->Bind(context);

            auto& mesh{ _scene._mesh[_scene._nodeId_to_meshId[node]] };
            mesh.Bind(context);
    //        if (_scene._nodeId_to_materialId.contains(node)) 
				//_scene._material[_scene._nodeId_to_materialId[node]].Bind(context);
			context.DrawIndexedInstanced(mesh.GetIndexCount(), 1u, 0u, 0u, 0u);
        }

        if ((_scene._tree[node]._firstChild != -1) && !_visited[_scene._tree[node]._firstChild]) {
            _visited[_scene._tree[node]._firstChild] = true;
            _stack.push_back(_scene._tree[node]._firstChild);
        }
        for (auto s{ _scene._tree[node]._nextSibling }; s != -1; s = _scene._tree[s]._nextSibling)
            if (!_visited[s]) {
                _visited[s] = true;
                _stack.push_back(s);
            }
    }
}

void DrawSceneStage::Update(ID3D11DeviceContext& context)
{
    if (ImNodes::IsNodeSelected(_id)) {
        ImGui::Begin("Scene Editor");
        ImGui::End();
    }
}

void DrawSceneStage::InitRS()
{
    D3D11_RASTERIZER_DESC rd{};

    // Default Solid
    {
        rd.FillMode = D3D11_FILL_MODE::D3D11_FILL_SOLID;
        rd.CullMode = D3D11_CULL_MODE::D3D11_CULL_BACK;
        rd.FrontCounterClockwise = false;
        rd.DepthClipEnable = true;
        rd.MultisampleEnable = false;

        pDevice->CreateRasterizerState(&rd, _rs.ReleaseAndGetAddressOf());
    }
}

void DrawSceneStage::InitDS()
{
    // default
    {
        D3D11_DEPTH_STENCIL_DESC desc{ CD3D11_DEPTH_STENCIL_DESC{CD3D11_DEFAULT{}} };

        pDevice->CreateDepthStencilState(&desc, _ds.ReleaseAndGetAddressOf());
    }

    D3D11_TEXTURE2D_DESC descDepth{};
    descDepth.Usage = D3D11_USAGE_DEFAULT;
    descDepth.BindFlags = D3D11_BIND_DEPTH_STENCIL;
    descDepth.Width = gViewportInfo.width;
    descDepth.Height = gViewportInfo.height;
    descDepth.MipLevels = 1u;
    descDepth.ArraySize = 1u;
    descDepth.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
    descDepth.SampleDesc.Count = 1;
    descDepth.SampleDesc.Quality = 0;

    Microsoft::WRL::ComPtr<ID3D11Texture2D> _ds;
    pDevice->CreateTexture2D(&descDepth, nullptr, _ds.ReleaseAndGetAddressOf());

    D3D11_DEPTH_STENCIL_VIEW_DESC desc_dsv{};
    desc_dsv.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
    desc_dsv.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
    desc_dsv.Texture2D.MipSlice = 0u;
    pDevice->CreateDepthStencilView(_ds.Get(), &desc_dsv, _dsv.ReleaseAndGetAddressOf());
}

void DrawSceneStage::InitBS()
{
    // Default
    {
        D3D11_BLEND_DESC desc{};
        desc.AlphaToCoverageEnable = false;
        desc.IndependentBlendEnable = false;

        desc.RenderTarget[0].BlendEnable = false;
        desc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
        desc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
        desc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;

        desc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ZERO;
        desc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
        desc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;

        desc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

        pDevice->CreateBlendState(&desc, _bs.ReleaseAndGetAddressOf());
    }
}
