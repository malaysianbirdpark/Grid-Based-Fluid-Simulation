#include "pch.h"
#include "CBFluid.h"

#include <cstdlib>
#include <ctime>

#include "imnodes.h"
#include "NodeManager.h"

CBFluid::CBFluid()
	: ConstantBufferStage{"Fluid Values"}
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

void CBFluid::Run(ID3D11DeviceContext& context)
{
    Update();
    Upload(context);
	context.CSSetConstantBuffers(0u, 1u, _buffer.GetAddressOf());
}

void CBFluid::Upload(ID3D11DeviceContext& context)
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

void CBFluid::Update()
{
    if (ImNodes::IsNodeSelected(_id)) {
        ImGui::Begin("Fluid Editor");
        ImGui::Text("Color");
        ImGui::RadioButton("Random Color", &color_mode, 0); ImGui::SameLine();
        ImGui::RadioButton("Fixed Color", &color_mode, 1); 
        ImGui::InputFloat("Color Scale", &_data._colorScale, 0.0f, 0.0f, "%.7f");
        if (color_mode == 1)
            ImGui::ColorPicker3("Fluid Color", &_data._color.x);

        ImGui::Text("Velocity");
        ImGui::RadioButton("Random Direction", &velocity_mode, 0); ImGui::SameLine();
        ImGui::RadioButton("Fixed Direction", &velocity_mode, 1); 
        ImGui::InputFloat("Speed", &_data._speed, 0.0f, 0.0f, "%.7f");
        if (velocity_mode == 1) 
            ImGui::DragFloat2("Fluid Velocity", &_data._velocityDir.x, 0.005f, -1.0f, 1.0f);

        ImGui::End();
    }

    if (color_mode == 0) {
		DirectX::XMVECTOR const random_color{
			static_cast<float>(std::rand()) / RAND_MAX,
			static_cast<float>(std::rand()) / RAND_MAX,
			static_cast<float>(std::rand()) / RAND_MAX,
            1.0f
        };

		DirectX::XMStoreFloat4(&_data._color, random_color);
    }

    if (velocity_mode == 0) {
		DirectX::XMVECTOR const random_velocity{
			static_cast<float>(std::rand() - (RAND_MAX >> 1)) / (RAND_MAX >> 2),
            -1.0f,
            0.0f,
            0.0f
        };

		DirectX::XMStoreFloat2(&_data._velocityDir, random_velocity);
    }
}
