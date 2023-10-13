#include "pch.h"
#include "ObjectRenderStage.h"

#include "RenderObject.h"

ObjectRenderStage::ObjectRenderStage(std::shared_ptr<RenderObject> object)
	: _object{object}
{
}

void ObjectRenderStage::Run(ID3D11DeviceContext& context)
{
	_object->Draw(context);
}
