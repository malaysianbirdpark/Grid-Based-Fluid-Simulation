#pragma once

#include <string>
#include <unordered_map>

class ResourceStage {
public: explicit ResourceStage(char const* name);
public: virtual ~ResourceStage() = default;
public: virtual ID3D11Resource* Expose(int32_t attribute_id = 0) const = 0;
public: void RenderNode() const;
public:
    int32_t                                 _id{-1};
    std::string                             _name{};
    inline static std::string               _stageName{ "Resource" };
public: std::unordered_map<int32_t, int32_t> _incoming{};
public: std::unordered_map<int32_t, int32_t> _outgoing{};
public:  std::unordered_map<int32_t, std::string> _attrNames;
};

