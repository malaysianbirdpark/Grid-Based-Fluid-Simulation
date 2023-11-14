#include "pch.h"
#include "CBViscosity.h"

#include "imgui.h"
#include "imnodes.h"
#include "NodeManager.h"

CBViscosity::CBViscosity(ID3D11DeviceContext& context)
	: ConstantBufferStage{"Viscosity"}
{
    _csID = NodeManager::IssueOutgoingAttrID();
    _outgoing[_csID] = -1;
    _attrNames[_csID] = { "Linked CS" };

    D3D11_BUFFER_DESC bd{};
    bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    bd.Usage = D3D11_USAGE_DYNAMIC;
    bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    bd.MiscFlags = 0;
    bd.ByteWidth = sizeof(Constants);
    bd.StructureByteStride = 0u;

    D3D11_SUBRESOURCE_DATA sd{};
    sd.pSysMem = &_data;

    pDevice->CreateBuffer(&bd, &sd, _buffer.ReleaseAndGetAddressOf());

    Upload(context);
}

void CBViscosity::Run(ID3D11DeviceContext& context)
{
    Update();
    Upload(context);
    //context.CSSetConstantBuffers(2u, 1u, _buffer.GetAddressOf());
}

void CBViscosity::Upload(ID3D11DeviceContext& context)
{
    D3D11_MAPPED_SUBRESOURCE msr{};
    context.Map(
        _buffer.Get(),
        0u,
        D3D11_MAP_WRITE_DISCARD,
        0u,
        &msr
    );
    memcpy(msr.pData, &_data, sizeof(Constants));
    context.Unmap(_buffer.Get(), 0u);
}

void CBViscosity::Update()
{
    if (ImNodes::IsNodeSelected(_id)) {
        ImGui::Begin("Viscosity Editor");
        ImGui::DragFloat("Viscosity", &_data.viscosity, 0.005f, 0.0015f, 50.0f);
        ImGui::Text("Alpha: %f", _data.alpha);
        ImGui::Text("Beta: %f", 1.0f / _data.r_beta);
        ImGui::End();
    }
    _data.dt = 1.0f / ImGui::GetIO().Framerate;
    _data.alpha = 1.0f / (_data.viscosity * _data.dt);
    _data.r_beta = 1.0f / (6.0f + _data.alpha);
}
