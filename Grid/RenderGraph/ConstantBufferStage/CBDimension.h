#pragma once

#include "ConstantBufferStage.h"

class CBDimension final : public ConstantBufferStage
{
	struct Constants {
		uint32_t _width;
		uint32_t _height;
		uint32_t _depth;
		float _reciprocal_width;
		float _reciprocal_height;
		float _reciprocal_depth;
		float pad[2];
	};

public: CBDimension(ID3D11DeviceContext& context);
public: ~CBDimension() override = default;

public: virtual void Run(ID3D11DeviceContext& context) override;
public: virtual void Upload(ID3D11DeviceContext& context) override;
public: virtual void Update() override;

private: Constants _data;
};

