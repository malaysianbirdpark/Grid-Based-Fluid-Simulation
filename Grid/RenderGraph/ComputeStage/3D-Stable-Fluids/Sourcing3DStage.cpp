#include "pch.h"
#include "Sourcing3DStage.h"

Sourcing3DStage::Sourcing3DStage()
	: Compute3DStage{"3D-Sourcing", "./CSO/Sourcing3D_CS.cso", 16, 8, 8}
{
}

void Sourcing3DStage::Run(ID3D11DeviceContext& context)
{
}

void Sourcing3DStage::Consume(ID3D11Resource* resource, int32_t attribute_id)
{
}

ID3D11Resource* Sourcing3DStage::Expose(int32_t attribute_id)
{
	return nullptr;
}
