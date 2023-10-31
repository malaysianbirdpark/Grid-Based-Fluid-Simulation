#include "pch.h"
#include "CBTimestep.h"

#include "imgui.h"
#include "NodeManager.h"

CBTimestep::CBTimestep()
	: ConstantBufferStage{"Timestep"}
{
    _csID = NodeManager::IssueOutgoingAttrID();
    _outgoing[_csID] = -1;
    _attrNames[_csID] = { "Linked CS" };

    _data._dt = 0.1f;

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
}

void CBTimestep::Run(ID3D11DeviceContext& context)
{
    Update();
    Upload(context);
	context.CSSetConstantBuffers(1u, 1u, _buffer.GetAddressOf());
}

void CBTimestep::Upload(ID3D11DeviceContext& context)
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

void CBTimestep::Update()
{
    //_data._dt = 1.0f / ImGui::GetIO().Framerate;
    _data._dt = 0.0166f;
}
