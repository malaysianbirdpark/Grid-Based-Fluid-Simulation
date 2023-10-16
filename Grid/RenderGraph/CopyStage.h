#pragma once

#include <string>
#include <vector>

class CopyStage {
public: explicit CopyStage();
public: virtual void     Run(ID3D11DeviceContext& context);
public: void     Consume(ID3D11Resource* resource, int32_t attribute_id);
public: void     RenderNode() const;

public: int32_t                     _id{-1};
public: std::string                 _name{"CopyStage"};
public: inline static std::string   _stageName{ "CopyStage" };
public: std::vector<int32_t>        _incoming{};
public: std::vector<int32_t>        _outgoing{};

protected: ID3D11Resource* _src;
protected: ID3D11Resource* _dest;
};


class ResolveStage final : public CopyStage {
public: explicit ResolveStage();
public: ~ResolveStage() = default;
public: virtual void Run(ID3D11DeviceContext& context) override;
};
