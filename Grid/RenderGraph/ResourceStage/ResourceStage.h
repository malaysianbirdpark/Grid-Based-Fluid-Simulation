#pragma once

#include <string>
#include <vector>

class ResourceStage {
public: explicit ResourceStage(char const* name);
public: virtual ~ResourceStage() = default;
public: virtual ID3D11Resource* Expose(int32_t attribute_id = 0) const = 0;
public: virtual void RenderNode() const = 0;
public:
    int32_t                                 _id{-1};
    std::string                             _name{};
    inline static std::string               _stageName{ "Resource" };
    std::vector<int32_t>                    _incoming{};
    std::vector<int32_t>                    _outgoing{};
};

