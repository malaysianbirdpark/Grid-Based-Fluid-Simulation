#pragma once

#include <vector>

class RenderObject {
public: virtual void Update(ID3D11DeviceContext& context, float const dt) = 0;
public: virtual void Draw(ID3D11DeviceContext& context) = 0;
public: virtual std::vector<D3D11_INPUT_ELEMENT_DESC> const& GetInputElementDest() const = 0;
};

