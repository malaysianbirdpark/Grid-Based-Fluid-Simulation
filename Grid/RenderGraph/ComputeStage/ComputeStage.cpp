#include "pch.h"
#include "ComputeStage.h"

#include <string>
#include <d3dcompiler.h>

#include "imgui.h"
#include "imnodes.h"

ComputeStage::ComputeStage(char const* name, char const* compute_shader_path, UINT group_x, UINT group_y, UINT group_z)
    : _name{name}, _groupX{ group_x }, _groupY{ group_y }, _groupZ{ group_z }
{
    std::string _path{ compute_shader_path };
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
        _cs.ReleaseAndGetAddressOf()
    );
}

void ComputeStage::Run(ID3D11DeviceContext& context) {
    context.OMSetRenderTargets(0u, nullptr, nullptr);
	context.CSSetShaderResources(0u, static_cast<UINT>(_srv.size()), _srv[0].GetAddressOf());
	context.CSSetUnorderedAccessViews(0u, static_cast<UINT>(_uav.size()), _uav[0].GetAddressOf(), nullptr);
	context.CSSetShader(_cs.Get(), nullptr, 0u);
	context.Dispatch(_groupX, _groupY, _groupZ);
	SetBarrier(context);
}

void ComputeStage::RenderNode() const {
    ImNodes::BeginNode(_id);
    ImNodes::BeginNodeTitleBar();
    ImGui::Text(_stageName.c_str());
    ImGui::Text(_name.c_str());
    ImGui::Text("%d", _id);
    ImNodes::EndNodeTitleBar();

    ImNodes::BeginInputAttribute(_id);
    ImGui::Text("Incoming");
    ImNodes::EndInputAttribute();

    for (auto& child : _outgoing) {
        ImNodes::BeginOutputAttribute(child << 8);
        ImGui::Text("Outgoing %d", child);
        ImNodes::EndOutputAttribute();
    }

    ImNodes::EndNode();
}

void ComputeStage::SetBarrier(ID3D11DeviceContext& context) const {
    context.CSSetShaderResources(0, static_cast<UINT>(_nullSrv.size()), &_nullSrv[0]);
    context.CSSetUnorderedAccessViews(0, static_cast<UINT>(_nullUav.size()), &_nullUav[0], nullptr);
}
