#include "pch.h"
#include "ObjectRenderStage.h"

#include "RenderObject.h"

ObjectRenderStage::ObjectRenderStage(char const* name, std::shared_ptr<RenderObject> object)
	: _name{name}, _object {object}
{
}

void ObjectRenderStage::Run(ID3D11DeviceContext& context)
{
	_object->Draw(context);
}
