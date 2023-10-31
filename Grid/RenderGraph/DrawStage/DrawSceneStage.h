#pragma once

#include "DrawStage.h"

class DrawSceneStage final : public DrawStage {
public: explicit DrawSceneStage();
public: virtual ~DrawSceneStage() override = default;
};
