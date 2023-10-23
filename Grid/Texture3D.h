#pragma once

class Texture3D {
public: explicit Texture3D();
public: void     Bind(ID3D11DeviceContext& context);
private:
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> _srv;
};

