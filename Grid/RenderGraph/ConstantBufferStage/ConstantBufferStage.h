#pragma once

#include <unordered_map>

class ConstantBufferStage
{
public: explicit ConstantBufferStage(char const* name);
public: virtual ~ConstantBufferStage() = default;
public: virtual void Run(ID3D11DeviceContext& context) = 0;
public: virtual void Upload(ID3D11DeviceContext& context) = 0;
public: virtual void Update() = 0;
public: void    RenderNode() const;
public:
    int32_t                                 _id{-1};
    std::string                             _name{};
    inline static std::string               _stageName{ "ConstantBuffer" };
public: std::unordered_map<int32_t, int32_t> _incoming{};
public: std::unordered_map<int32_t, int32_t> _outgoing{};
public: std::unordered_map<int32_t, std::string> _attrNames;

protected: Microsoft::WRL::ComPtr<ID3D11Buffer> _buffer;
};

