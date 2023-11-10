#include "pch.h"
#include "DrawSceneStage.h"

#include "imnodes.h"

#include "Transform.h"
#include "PipelineStateObject.h"

#include "Transform.h"

DrawSceneStage::DrawSceneStage(ID3D11DeviceContext& context, char const* name)
	: DrawStage{name}
{
    _scene.Init(context, "./Assets/Avocado/Avocado.gltf", "Avocado");
    _transform = std::make_unique<Transform>(context, DirectX::XMMatrixIdentity());

    _stack.resize(_scene._tree.size());
    _visited.resize(_scene._tree.size());

    std::vector<D3D11_INPUT_ELEMENT_DESC> input_elem_desc{
        {"POSITION", 0u, DXGI_FORMAT_R32G32B32_FLOAT, 0u, 0u, D3D11_INPUT_PER_VERTEX_DATA, 0u},
        {"NORMAL", 0u, DXGI_FORMAT_R32G32B32_FLOAT, 0u, sizeof(DirectX::XMFLOAT3), D3D11_INPUT_PER_VERTEX_DATA, 0u},
        {"TANGENT", 0u, DXGI_FORMAT_R32G32B32_FLOAT, 0u, sizeof(DirectX::XMFLOAT3) * 2, D3D11_INPUT_PER_VERTEX_DATA, 0u},
        {"TEXCOORD", 0u, DXGI_FORMAT_R32G32_FLOAT, 0u, sizeof(DirectX::XMFLOAT3) * 3, D3D11_INPUT_PER_VERTEX_DATA, 0u},
    };

    _pso.push_back(std::move(std::make_unique<PipelineStateObject>()));
	_pso.back()->SetVertexShader("./CSO/Phong_VS.cso");
	_pso.back()->SetInputLayout(input_elem_desc);
	_pso.back()->SetPixelShader("./CSO/Phong_PS.cso");

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
            if (_scene._nodeId_to_materialId.contains(node)) 
				_scene._material[_scene._nodeId_to_materialId[node]].Bind(context);
			context.DrawIndexedInstanced(mesh.GetIndexCount(), 1u, 0u, 0u, 0u);
            mesh.CalculateVelocity(context);
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
        int interacted{};

        auto& transform{ _scene.GetTransformParamAt(0) };

        ImGui::Text("Position");
        interacted += ImGui::SliderFloat("X", &transform.x, -20.0f, 20.0f);
        interacted += ImGui::SliderFloat("Y", &transform.y, -20.0f, 20.0f);
        interacted += ImGui::SliderFloat("Z", &transform.z, -20.0f, 20.0f);

        ImGui::Text("Rotation");
        interacted += ImGui::SliderAngle("Roll", &transform.roll, -180.0f, 180.0f);
        interacted += ImGui::SliderAngle("Pitch", &transform.pitch, -180.0f, 180.0f);
        interacted += ImGui::SliderAngle("Yaw", &transform.yaw, -180.0f, 180.0f);

        ImGui::Text("Scaling");
        interacted += ImGui::SliderFloat("Scale", &transform.scale, 0.1f, 10.0f);

        if (interacted > 0)
            _scene.MarkAsTransformed(0);

        ImGui::End();
    }

    _scene.Update();
}

void DrawSceneStage::RawDrawInstanced(ID3D11DeviceContext& context, UINT num_of_instance)
{
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
            mesh.RawBind(context);
			context.DrawIndexedInstanced(mesh.GetIndexCount(), num_of_instance, 0u, 0u, 0u);
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
