#include "pch.h"
#include "CBFluidColor.h"

#include <cstdlib>
#include <ctime>

#include "NodeManager.h"

CBFluidColor::CBFluidColor()
	: ConstantBufferStage{"Fluid Color"}
{
    _csID = NodeManager::IssueOutgoingAttrID();
    _outgoing[_csID] = -1;
    _attrNames[_csID] = { "Linked CS" };

    DirectX::XMStoreFloat4(&_data._color, DirectX::XMVECTOR{});

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

    std::srand(std::time(nullptr));
}

void CBFluidColor::Run(ID3D11DeviceContext& context)
{
    Update();
    Upload(context);
	context.CSSetConstantBuffers(0u, 1u, _buffer.GetAddressOf());
}

void CBFluidColor::Upload(ID3D11DeviceContext& context)
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

void CBFluidColor::Update()
{
    DirectX::XMVECTOR const random_color {
        static_cast<float>(std::rand()) / RAND_MAX,
        static_cast<float>(std::rand()) / RAND_MAX,
        static_cast<float>(std::rand()) / RAND_MAX,
        1.0f
    };
    DirectX::XMStoreFloat4(&_data._color, random_color);
}
