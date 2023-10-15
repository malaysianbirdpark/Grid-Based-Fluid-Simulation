#pragma once

#include <vector>

class PSTextures
{
public:
	enum class Type {
		Diffuse,
		Normal,
	};
public: explicit PSTextures() = default;
public: void     AddTexture(ID3D11DeviceContext& context, char const* path, Type texture_type);
public: void     Bind(ID3D11DeviceContext& context);
private:
	std::vector < Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> > _srv;
};

