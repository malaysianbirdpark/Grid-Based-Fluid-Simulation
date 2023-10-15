#pragma once

#include <string>
#include <vector>

class ResourceStage {
public: explicit ResourceStage(char const* name, ID3D11Device& device);
private:
    int32_t                    _parent{-1};
    int32_t                    _id{0};
    std::string                _name{};
    inline static std::string  _stageName{ "Resource" };
    std::vector<int32_t>       _child{};
};

