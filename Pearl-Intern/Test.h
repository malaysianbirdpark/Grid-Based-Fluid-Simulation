#pragma once

#include <vector>
#include <memory>

class Test {
	struct Vertex {
		DirectX::XMFLOAT3 _pos;
		DirectX::XMFLOAT3 _color;
	};
public:
	explicit Test(ID3D11Device& device);
	void     Draw(ID3D11DeviceContext& context);
private:
	std::unique_ptr<class PipelineStateObject> _pso;
private:
	UINT _indexCount;
	Microsoft::WRL::ComPtr<ID3D11Buffer>   _indexBuffer;

	D3D11_PRIMITIVE_TOPOLOGY               _topology;
	std::vector<D3D11_INPUT_ELEMENT_DESC>  _inputElementDesc{};
	Microsoft::WRL::ComPtr<ID3D11Buffer>   _vertexBuffer;
};

