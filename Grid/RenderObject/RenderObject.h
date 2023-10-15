#pragma once

class RenderObject {
public: virtual void Update(ID3D11DeviceContext& context, float const dt) = 0;
public: virtual void Draw(ID3D11DeviceContext& context) = 0;
};

