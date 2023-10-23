#pragma once

#include <unordered_map>
#include <string>
#include <memory>

class DrawStage
{
public:  explicit DrawStage(char const* name, std::shared_ptr<class RenderObject> object);

public:  int32_t                    _id{-1};
public:  std::string                _name{};
public:  inline static std::string  _stageName{ "DrawStage" };
public:  std::unordered_map<int32_t, int32_t> _incoming{};
public:  std::unordered_map<int32_t, int32_t> _outgoing{};
public:  std::unordered_map<int32_t, std::string> _attrNames;

public:  void Run(ID3D11DeviceContext& context);
public:  void Update(ID3D11DeviceContext& context);
public:  void RenderNode() const;
public:  void Consume(ID3D11Resource* resource, int32_t attribute_id);
public:  ID3D11Resource* Expose(int32_t attribute_id);

private: virtual void InitRS();
private: virtual void InitDS();
private: virtual void InitBS();

private: std::shared_ptr<RenderObject> _object;

private: Microsoft::WRL::ComPtr<ID3D11Texture2D>            _resource;
private: Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>   _srv;
private: Microsoft::WRL::ComPtr<ID3D11RenderTargetView>     _rtv;

protected: Microsoft::WRL::ComPtr<ID3D11RasterizerState>    _rs;

protected: Microsoft::WRL::ComPtr<ID3D11DepthStencilState>  _ds;
protected: Microsoft::WRL::ComPtr<ID3D11DepthStencilView>   _dsv;

protected: Microsoft::WRL::ComPtr<ID3D11BlendState>         _bs;

protected: D3D11_VIEWPORT                                   _vp{};

private: int32_t _resultID{};
protected: static float constexpr clear_color[4]{ 0.0f, 0.0f, 0.0f, 1.0f };
};

