#include "pch.h"
#include "CBDimension.h"

#include "imnodes.h"
#include "NodeManager.h"

CBDimension::CBDimension(ID3D11DeviceContext& context)
	: ConstantBufferStage{"Dimensions"}
{
    _data._width = static_cast<uint32_t>(gSimulationInfo.width);
    _data._height = static_cast<uint32_t>(gSimulationInfo.height);
    _data._depth = static_cast<uint32_t>(gSimulationInfo.depth);
    _data._reciprocal_width = static_cast<float>(1.0f / gSimulationInfo.height);
    _data._reciprocal_height = static_cast<float>(1.0f / gSimulationInfo.height);
    _data._reciprocal_depth = static_cast<float>(1.0f / gSimulationInfo.height);

    D3D11_BUFFER_DESC bd{};
    bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    bd.Usage = D3D11_USAGE_IMMUTABLE;
    bd.CPUAccessFlags = 0u;
    bd.MiscFlags = 0;
    bd.ByteWidth = sizeof(Constants);
    bd.StructureByteStride = 0u;

    D3D11_SUBRESOURCE_DATA sd{};
    sd.pSysMem = &_data;

    pDevice->CreateBuffer(&bd, &sd, _buffer.ReleaseAndGetAddressOf());
}

void CBDimension::Run(ID3D11DeviceContext& context)
{
	context.CSSetConstantBuffers(3u, 1u, _buffer.GetAddressOf());
	context.PSSetConstantBuffers(3u, 1u, _buffer.GetAddressOf());
}

void CBDimension::Upload(ID3D11DeviceContext& context)
{
}

void CBDimension::Update()
{
}

