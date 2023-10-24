#include "pch.h"
#include "Texture3DTest.h"

#include "NodeManager.h"

Texture3DTest::Texture3DTest()
	: Compute3DStage{"Test", "./CSO/3DTextureTest_CS.cso", 16, 8, 8}
{
    _uav.resize(1);
    _srv.resize(1);
    _nullUav.resize(1);
    _nullSrv.resize(1);

    _resource.resize(1);

	auto desc {CD3D11_TEXTURE3D_DESC{}};
	desc.BindFlags = D3D11_BIND_UNORDERED_ACCESS | D3D11_BIND_SHADER_RESOURCE;
	desc.Usage = D3D11_USAGE_DEFAULT;
	desc.CPUAccessFlags = 0u;
	desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	desc.Width = gViewportInfo.width;
	desc.Height = gViewportInfo.height;
	desc.Depth = gViewportInfo.depth;
	desc.MipLevels = 1u;

    pDevice->CreateTexture3D(&desc, nullptr, _resource[0].ReleaseAndGetAddressOf());
    pDevice->CreateUnorderedAccessView(_resource[0].Get(), nullptr, _uav[0].ReleaseAndGetAddressOf());

    _resultID = NodeManager::IssueOutgoingAttrID();
    _outgoing[_resultID] = -1;
    _attrNames[_resultID] = { "Result" };

}

void Texture3DTest::Run(ID3D11DeviceContext& context)
{
    Compute3DStage::Run(context);
}

void Texture3DTest::Consume(ID3D11Resource* resource, int32_t attribute_id)
{
}

ID3D11Resource* Texture3DTest::Expose(int32_t attribute_id)
{
    if (attribute_id == _resultID)
        return static_cast<ID3D11Resource*>(_resource[0].Get());
	return nullptr;
}
