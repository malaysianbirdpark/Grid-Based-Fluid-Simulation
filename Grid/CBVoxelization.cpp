#include "pch.h"
#include "CBVoxelization.h"

#include "imnodes.h"
#include "NodeManager.h"

CBVoxelization::CBVoxelization()
	: ConstantBufferStage{"Voxelization"}
{
    _data.width = gSimulationInfo.width;
    _data.height = gSimulationInfo.height;

    D3D11_BUFFER_DESC bd{};
    bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    bd.Usage = D3D11_USAGE_DYNAMIC;
    bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    bd.MiscFlags = 0;
    bd.ByteWidth = sizeof(Constant);
    bd.StructureByteStride = 0u;

    D3D11_SUBRESOURCE_DATA sd{};
    sd.pSysMem = &_data;

    pDevice->CreateBuffer(&bd, &sd, _buffer.ReleaseAndGetAddressOf());
}

void CBVoxelization::Run(ID3D11DeviceContext& context)
{
    Update();
    Upload(context);
	context.GSSetConstantBuffers(0u, 1u, _buffer.GetAddressOf());
}

void CBVoxelization::Upload(ID3D11DeviceContext& context)
{
    D3D11_MAPPED_SUBRESOURCE msr{};
    context.Map(
        _buffer.Get(),
        0u,
        D3D11_MAP_WRITE_DISCARD,
        0u,
        &msr
    );
    memcpy(msr.pData, &_data, sizeof(Constant));
    context.Unmap(_buffer.Get(), 0u);
}

void CBVoxelization::Update()
{
}

