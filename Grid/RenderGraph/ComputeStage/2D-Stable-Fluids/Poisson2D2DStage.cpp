#include "pch.h"
#include "Poisson2D2DStage.h"

#include <d3dcompiler.h>

#include "imnodes.h"
#include "NodeManager.h"

Poisson2D2DStage::Poisson2D2DStage()
	: ComputeStage{"2D2D-Possion Solver", "./CSO/Poisson2D2D_CS.cso", 32, 32, 1}
{
    _uav.resize(1);
    _srv.resize(3);
    _nullUav.resize(1);
    _nullSrv.resize(3);

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
    pDevice->CreateUnorderedAccessView(_resource[0].Get(), nullptr, _uav[0].ReleaseAndGetAddressOf());
    pDevice->CreateShaderResourceView(_resource[0].Get(), nullptr, _srv[2].ReleaseAndGetAddressOf());

    _xInID = NodeManager::IssueIncomingAttrID();
    _incoming[_xInID] = -1;
    _attrNames[_xInID] = { "x" };

    _bID = NodeManager::IssueIncomingAttrID();
    _incoming[_bID] = -1;
    _attrNames[_bID] = { "b" };

    _constID = NodeManager::IssueIncomingAttrID();
    _incoming[_constID] = -1;
    _attrNames[_constID] = { "Constants(CB)" };

    _xOutID = NodeManager::IssueOutgoingAttrID();
    _outgoing[_xOutID] = -1;
    _attrNames[_xOutID] = { "new x" };

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

    pDevice->CreateTexture2D(&desc, nullptr, _resultUnorm.ReleaseAndGetAddressOf());
    pDevice->CreateUnorderedAccessView(_resultUnorm.Get(), nullptr, _resultUnormView.ReleaseAndGetAddressOf());
    pDevice->CreateShaderResourceView(_resultUnorm.Get(), nullptr, _resultUnormSR.ReleaseAndGetAddressOf());
}

void Poisson2D2DStage::Run(ID3D11DeviceContext& context)
{
    ID3D11Resource* src {nullptr};
    ID3D11Resource* dest {nullptr};
    for (auto i {0}; i != 31; ++i) {
        ComputeStage::Run(context);

        if (_uav[0].Get() && _srv[0].Get()) {
			_uav[0]->GetResource(&src);
			_srv[0]->GetResource(&dest);
			context.CopyResource(dest, src);
        }
    }

    if (ImNodes::IsNodeSelected(_id)) {
        ImGui::Begin("Node Editor");

        context.CSSetShaderResources(0u, 1u, _srv[2].GetAddressOf());
        context.CSSetUnorderedAccessViews(0u, 1u, _resultUnormView.GetAddressOf(), nullptr);
        context.CSSetShader(_normalizer.Get(), nullptr, 0u);
        context.Dispatch(
            static_cast<UINT>(ceil(static_cast<float>(gViewportInfo.width) / _groupX)),
            static_cast<UINT>(ceil(static_cast<float>(gViewportInfo.height) / _groupY)), 
            _groupZ
        );
        SetBarrier(context);

        ImGui::Text("Result");
        ImGui::Image(
            _resultUnormSR.Get(), 
            ImVec2{ 
                static_cast<float>(gViewportInfo.width >> 1),
                static_cast<float>(gViewportInfo.height >> 1)
            }
        );

        ImGui::End();
    }
}

void Poisson2D2DStage::Consume(ID3D11Resource* resource, int32_t attribute_id)
{
    if (attribute_id == _xInID)
		pDevice->CreateShaderResourceView(resource, nullptr, _srv[0].ReleaseAndGetAddressOf());
    else if (attribute_id == _bID)
        pDevice->CreateShaderResourceView(resource, nullptr, _srv[1].ReleaseAndGetAddressOf());
}

ID3D11Resource* Poisson2D2DStage::Expose(int32_t attribute_id)
{
    if (attribute_id == _xOutID)
		return static_cast<ID3D11Resource*>(_resource[0].Get());
}
