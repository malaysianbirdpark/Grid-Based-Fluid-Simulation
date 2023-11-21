#pragma once

#include <unordered_map>
#include <string>
#include <memory>

class DrawStage
{
public:  explicit DrawStage(char const* name);
public:  explicit DrawStage(char const* name, std::shared_ptr<class RenderObject> object);
public:  virtual ~DrawStage() = default;

public:  int32_t                    _id{-1};
public:  std::string                _name{};
public:  inline static std::string  _stageName{ "DrawStage" };
public:  std::unordered_map<int32_t, int32_t> _incoming{};
public:  std::unordered_map<int32_t, int32_t> _outgoing{};
public:  std::unordered_map<int32_t, std::string> _attrNames;

public:  virtual void     Run(ID3D11DeviceContext& context);
public:  virtual void     Update(ID3D11DeviceContext& context);
public:  virtual void     RenderNode() const;
public:  virtual void     Consume(ID3D11Resource* resource, int32_t attribute_id);
public:  ID3D11Resource*  Expose(int32_t attribute_id);

protected: virtual void InitRS();
protected: virtual void InitDS();
protected: virtual void InitBS();

protected: std::shared_ptr<RenderObject>                           _object;
protected: std::vector<std::unique_ptr<class PipelineStateObject>> _pso;

// input
protected: Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>  _previous;

// result
protected: Microsoft::WRL::ComPtr<ID3D11Texture2D>           _resultBuffer;
protected: Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>  _resultSRV;
protected: Microsoft::WRL::ComPtr<ID3D11RenderTargetView>    _resultRTV;

protected: Microsoft::WRL::ComPtr<ID3D11RasterizerState>     _rs;

protected: Microsoft::WRL::ComPtr<ID3D11DepthStencilState>   _ds;
protected: Microsoft::WRL::ComPtr<ID3D11DepthStencilView>    _dsv;

protected: Microsoft::WRL::ComPtr<ID3D11BlendState>          _bs;

protected: D3D11_VIEWPORT                                    _vp{};

protected: int32_t _inputID{};
private:   int32_t _resultID{};
protected: static float constexpr clear_color[4]{ 0.6f, 0.6f, 0.6f, 1.0f };
};

