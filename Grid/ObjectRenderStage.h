#pragma once

#include <vector>
#include <memory>

class ObjectRenderStage
{
public:  explicit ObjectRenderStage(std::shared_ptr<class RenderObject> object);

public:  int32_t                _parent{-1};
public:  std::vector<int32_t>   _child{};
public:  int32_t                _id{-1};

public:  void Run(ID3D11DeviceContext& context);

private: std::shared_ptr<RenderObject> _object;
};

