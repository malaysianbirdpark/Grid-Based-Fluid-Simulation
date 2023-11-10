#pragma once

#include "DrawStage.h"
#include "SceneGraph.h"

class DrawSceneStage final : public DrawStage {
public: explicit  DrawSceneStage(ID3D11DeviceContext& context, char const* name);
public: virtual  ~DrawSceneStage() override = default;
public: virtual  void Run(ID3D11DeviceContext& context) override;
public: virtual  void Update(ID3D11DeviceContext& context) override;
public: void     RawDrawInstanced(ID3D11DeviceContext& context, UINT num_of_instance);
private: virtual void InitBS() override;
private:
	SceneGraph                        _scene{};
    std::unique_ptr<class Transform>  _transform;
    std::vector<int32_t>              _stack{};
    std::vector<bool>                 _visited{};
};
