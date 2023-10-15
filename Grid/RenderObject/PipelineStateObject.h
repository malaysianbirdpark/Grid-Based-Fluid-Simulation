#pragma once

#include <vector>

class PipelineStateObject {
public: void SetInputLayout(std::vector<D3D11_INPUT_ELEMENT_DESC> const& layout);
public: void SetVertexShader(char const* path);
public: void SetGeometryShader(char const* path);
public: void SetPixelShader(char const* path);
public: void Bind(ID3D11DeviceContext& context);
private:
    Microsoft::WRL::ComPtr<ID3D11InputLayout>     _inputLayout{};
    Microsoft::WRL::ComPtr<ID3D11VertexShader>    _vs{};
    Microsoft::WRL::ComPtr<ID3D11GeometryShader>  _gs{};
    Microsoft::WRL::ComPtr<ID3D11PixelShader>     _ps{};
    Microsoft::WRL::ComPtr<ID3DBlob>              _vsByteCode{};
};
 