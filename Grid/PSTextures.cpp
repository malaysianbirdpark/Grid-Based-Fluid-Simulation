#include "pch.h"
#include "PSTextures.h"

#include <string>
#include "DirectXTK-main\Inc\DDSTextureLoader.h"

void PSTextures::AddTexture(ID3D11Device& device, ID3D11DeviceContext& context, char const* path, Type texture_type)
{
	if (_srv.size() <= static_cast<int>(texture_type)) _srv.resize(static_cast<size_t>(texture_type) + 1);

    using namespace DirectX;

    std::string _path{ path };
    std::wstring p(_path.length(), L' ');
    std::ranges::copy(_path, p.begin());

    auto alphaMode{ DDS_ALPHA_MODE_STRAIGHT };

    auto const idx{ static_cast<size_t>(texture_type) };
    CreateDDSTextureFromFileEx(
        &device,
        p.c_str(),
        0,
        D3D11_USAGE_DEFAULT,
        D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET,
        D3D11_CPU_ACCESS_READ,
        D3D11_RESOURCE_MISC_GENERATE_MIPS,
        DDS_LOADER_FORCE_SRGB,
        nullptr,
        _srv[idx].ReleaseAndGetAddressOf(),
        &alphaMode
    );

    context.GenerateMips(_srv[idx].Get());
}

void PSTextures::Bind(ID3D11DeviceContext& context)
{
    context.PSSetShaderResources(0u, static_cast<UINT>(_srv.size()), _srv[0].GetAddressOf());
}
