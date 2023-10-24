#include "pch.h"
#include "GradientSubtract2DStage.h"

#include <d3dcompiler.h>

#include "imnodes.h"
#include "NodeManager.h"

GradientSubtract2DStage::GradientSubtract2DStage()
	: Compute2DStage{"2D-GradientSubtract", "./CSO/GradientSub2D_CS.cso", 32, 32, 1}
{
    _uav.resize(3);
    _srv.resize(1);
    _nullUav.resize(5);
    _nullSrv.resize(5);

    _resource.resize(1);

	auto desc {CD3D11_TEXTURE2D_DESC{}};
	desc.BindFlags = D3D11_BIND_UNORDERED_ACCESS | D3D11_BIND_SHADER_RESOURCE;
	desc.Usage = D3D11_USAGE_DEFAULT;
	desc.CPUAccessFlags = 0u;
	//desc.Format = DXGI_FORMAT_R16G16_FLOAT;
	desc.Format = DXGI_FORMAT_R16G16B16A16_FLOAT;
	desc.Width = gViewportInfo.width;
	desc.Height = gViewportInfo.height;
	desc.MipLevels = 1u;
	desc.ArraySize = 1u;
	desc.SampleDesc.Count = 1;
	desc.SampleDesc.Quality = 0;

    pDevice->CreateTexture2D(&desc, nullptr, _resource[0].ReleaseAndGetAddressOf());
    pDevice->CreateUnorderedAccessView(_resource[0].Get(), nullptr, _uav[2].ReleaseAndGetAddressOf());

    _xInID = NodeManager::IssueIncomingAttrID();
    _incoming[_xInID] = -1;
    _attrNames[_xInID] = { "x in" };

    _subTargetID = NodeManager::IssueIncomingAttrID();
    _incoming[_subTargetID] = -1;
    _attrNames[_subTargetID] = { "Subtract target" };

    _resultID = NodeManager::IssueOutgoingAttrID();
    _outgoing[_resultID] = -1;
    _attrNames[_resultID] = { "Result" };

    std::string _path{ "./CSO/Vector4DUnorm_CS.cso" };
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
        _normalizer.ReleaseAndGetAddressOf()
    );

    pDevice->CreateTexture2D(&desc, nullptr, _velocityUnorm.ReleaseAndGetAddressOf());
    pDevice->CreateUnorderedAccessView(_velocityUnorm.Get(), nullptr, _velocityUnormView.ReleaseAndGetAddressOf());
    pDevice->CreateShaderResourceView(_velocityUnorm.Get(), nullptr, _velocityUnormSR.ReleaseAndGetAddressOf());

}

void GradientSubtract2DStage::Run(ID3D11DeviceContext& context) {
    Compute2DStage::Run(context);

    if (ImNodes::IsNodeSelected(_id)) {
        ImGui::Begin("Node Editor");

        context.CSSetShaderResources(0u, 1u, _srv[0].GetAddressOf());
        context.CSSetUnorderedAccessViews(0u, 1u, _velocityUnormView.GetAddressOf(), nullptr);
        context.CSSetShader(_normalizer.Get(), nullptr, 0u);
        context.Dispatch(
            static_cast<UINT>(ceil(static_cast<float>(gViewportInfo.width) / _groupX)),
            static_cast<UINT>(ceil(static_cast<float>(gViewportInfo.height) / _groupY)), 
            _groupZ
        );
        SetBarrier(context);

        ImGui::Text("Velocity");
        ImGui::Image(
            _velocityUnormSR.Get(), 
            ImVec2{ 
                static_cast<float>(gViewportInfo.width >> 1),
                static_cast<float>(gViewportInfo.height >> 1)
            }
        );

        ImGui::End();
    }
}

void GradientSubtract2DStage::Consume(ID3D11Resource* resource, int32_t attribute_id)
{
    if (attribute_id == _xInID)
        pDevice->CreateUnorderedAccessView(resource, nullptr, _uav[0].ReleaseAndGetAddressOf());
    else if (attribute_id == _subTargetID)
        pDevice->CreateUnorderedAccessView(resource, nullptr, _uav[1].ReleaseAndGetAddressOf());
}

ID3D11Resource* GradientSubtract2DStage::Expose(int32_t attribute_id)
{
    if (attribute_id == _resultID)
        return static_cast<ID3D11Resource*>(_resource[0].Get());
	return nullptr;
}
