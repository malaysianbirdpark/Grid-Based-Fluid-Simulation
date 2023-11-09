#pragma once

#include "ConstantBufferStage.h"

class CBVoxelization final : public ConstantBufferStage
{
	friend class VoxelizationStage;
	struct Constant {
		uint32_t slice_index{};
		float    slice_depth{};
		uint32_t width{};
		uint32_t height{};
	};
public: CBVoxelization();
public: ~CBVoxelization() override = default;

public: virtual void Run(ID3D11DeviceContext& context) override;
public: virtual void Upload(ID3D11DeviceContext& context) override;
public: virtual void Update() override;

private: int32_t color_mode{};
private: int32_t velocity_mode{};
private: Constant _data;
};

