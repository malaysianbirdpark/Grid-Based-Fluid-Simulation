#pragma once

#include <vector>
#include <string>
#include <memory>

class DrawStage
{
public:  explicit DrawStage(char const* name, std::shared_ptr<class RenderObject> object);

public:  int32_t                    _id{-1};
public:  std::string                _name{};
public:  inline static std::string  _stageName{ "DrawStage" };
public:  std::vector<int32_t>       _incoming{};
public:  std::vector<int32_t>       _outgoing{};

public:  void Run(ID3D11DeviceContext& context);
public:  void RenderNode() const;

private: std::shared_ptr<RenderObject> _object;

};

