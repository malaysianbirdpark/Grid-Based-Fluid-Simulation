#pragma once

#include <vector>
#include <string>
#include <memory>

class ObjectRenderStage
{
public:  explicit ObjectRenderStage(char const* name, std::shared_ptr<class RenderObject> object);

public:  int32_t                    _parent{-1};
public:  int32_t                    _id{-1};
public:  std::string                _name{};
public:  inline static std::string  _stageName{ "ObjectRenderStage" };
public:  std::vector<int32_t>       _child{};

public:  void Run(ID3D11DeviceContext& context);

private: std::shared_ptr<RenderObject> _object;

};

