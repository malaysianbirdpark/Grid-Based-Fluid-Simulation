#include "pch.h"
#include "GradientSubtract2DStage.h"

#include "NodeManager.h"

GradientSubtract2DStage::GradientSubtract2DStage()
	: ComputeStage{"2D-GradientSubtract", "./CSO/GradientSub2D_CS.cso", 32, 32, 1}
{
    _uav.resize(2);
    _srv.resize(1);
    _nullUav.resize(2);
    _nullSrv.resize(1);

    _resource.resize(1);

	auto desc {CD3D11_TEXTURE2D_DESC{}};
	desc.BindFlags = D3D11_BIND_UNORDERED_ACCESS | D3D11_BIND_SHADER_RESOURCE;
	desc.Usage = D3D11_USAGE_DEFAULT;
	desc.CPUAccessFlags = 0u;
	desc.Format = DXGI_FORMAT_R16G16_FLOAT;
	desc.Width = gViewportInfo.width;
	desc.Height = gViewportInfo.height;
	desc.MipLevels = 1u;
	desc.ArraySize = 1u;
	desc.SampleDesc.Count = 1;
	desc.SampleDesc.Quality = 0;

    pDevice->CreateTexture2D(&desc, nullptr, _resource[0].ReleaseAndGetAddressOf());
    pDevice->CreateUnorderedAccessView(_resource[0].Get(), nullptr, _uav[1].ReleaseAndGetAddressOf());

    _xInID = NodeManager::IssueIncomingAttrID();
    _incoming[_xInID] = -1;
    _attrNames[_xInID] = { "x in" };

    _subTargetID = NodeManager::IssueIncomingAttrID();
    _incoming[_subTargetID] = -1;
    _attrNames[_subTargetID] = { "Subtract target" };

    _resultID = NodeManager::IssueOutgoingAttrID();
    _outgoing[_resultID] = -1;
    _attrNames[_resultID] = { "Result" };

}

void GradientSubtract2DStage::Run(ID3D11DeviceContext& context)
{
    ComputeStage::Run(context);
}

void GradientSubtract2DStage::Consume(ID3D11Resource* resource, int32_t attribute_id)
{
    if (attribute_id == _xInID)
        pDevice->CreateShaderResourceView(resource, nullptr, _srv[0].ReleaseAndGetAddressOf());
    else if (attribute_id == _subTargetID)
        pDevice->CreateUnorderedAccessView(resource, nullptr, _uav[0].ReleaseAndGetAddressOf());
}

ID3D11Resource* GradientSubtract2DStage::Expose(int32_t attribute_id)
{
    ID3D11Resource* resource;
    if (attribute_id == _resultID)
        _uav[1]->GetResource(&resource);
    return resource;
}
