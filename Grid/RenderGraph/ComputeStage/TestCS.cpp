#include "pch.h"
#include "TestCS.h"

TestCS::TestCS()
    : ComputeStage{"Test: Paint Whole Screen", "CSO/testCS.cso", 32, 32, 1}
{
    _uav.resize(1);
    _srv.resize(1);
    _nullUav.resize(1);
    _nullSrv.resize(1);
}

TestCS::~TestCS() {
}

void TestCS::Consume(ID3D11Resource* resource, int32_t attribute_id = 0) {
    if (attribute_id > 0) return;

    auto desc {CD3D11_UNORDERED_ACCESS_VIEW_DESC{}};
	desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	desc.ViewDimension = D3D11_UAV_DIMENSION_TEXTURE2D;

    pDevice->CreateUnorderedAccessView(resource, &desc, _uav[0].ReleaseAndGetAddressOf());
}

ID3D11Resource* TestCS::Expose(int32_t attribute_id) {
    ID3D11Resource* resource {nullptr};
    _uav[attribute_id]->GetResource(&resource);
    return resource;
}

void TestCS::RenderNode() const {
    ComputeStage::RenderNode();
}
