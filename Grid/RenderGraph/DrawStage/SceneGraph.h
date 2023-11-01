#pragma once

#include <vector>
#include <string>
#include <unordered_map>
#include <bitset>
#include <memory>

#include "assimp/cimport.h"
#include "assimp/scene.h"
#include "assimp/postprocess.h"

enum class ShaderResourceTypes {
    DiffuseMap,
    MetallicMap,
    RoughnessMap,
    NormalMap,
    EmissiveMap,
    OcclusionMap,
    ThicknessMap,
};

struct SceneTransformParameters {
    float x{};
    float y{};
    float z{};
    float roll{};
    float pitch{};
    float yaw{};
    float scale{ 1.0f };
};

class AssimpMaterial {
    friend class SceneGraph;
public:
    explicit AssimpMaterial();
    void     Bind(ID3D11DeviceContext& context);
    void     AddOrRelplaceTexture(ID3D11DeviceContext& context, ShaderResourceTypes type, char const* path);
private:
    DirectX::XMFLOAT4  _emissiveColor{};
    DirectX::XMFLOAT4  _albedoColor{};
    DirectX::XMFLOAT4  _roughness{};
    DirectX::XMFLOAT4  _attenuationColor{};
    float              _transparencyFactor{};
    float              _attenuationDistance{};
    float              _thicknessFactor{};
    float              _alphaTest{};
    float              _metallicFactor{};

    std::vector<Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>> _srs;
};

class AssimpMesh {
    friend class SceneGraph;
public:
    AssimpMesh() = default;
    AssimpMesh(
        Microsoft::WRL::ComPtr<ID3D11Buffer>&& vertex_buffer,
        Microsoft::WRL::ComPtr<ID3D11Buffer>&& index_buffer,
        D3D11_PRIMITIVE_TOPOLOGY topology,
        UINT index_count
    );

    void Bind(ID3D11DeviceContext & context) const;

    [[nodiscard]] UINT GetIndexCount() const;
private:
    Microsoft::WRL::ComPtr<ID3D11Buffer>  _vertexBuffer;
    Microsoft::WRL::ComPtr<ID3D11Buffer>  _indexBuffer;
    D3D11_PRIMITIVE_TOPOLOGY              _topology;
    UINT                                  _indexCount;
    UINT                                  _stride;
};

struct SceneNode {
    int32_t _parent;
    int32_t _firstChild;
    int32_t _firstSibling;
    int32_t _lastSibling;
    int32_t _level;

    int32_t _nextSibling{ -1 };
};

class SceneGraph {
    friend class DrawSceneStage;
    static constexpr int MAX_NODE_LEVEL{ 20 };
public:
    explicit SceneGraph() = default;
    explicit SceneGraph(ID3D11DeviceContext& context, char const* path, char const* tag);

    void                                    Init(ID3D11DeviceContext& context, char const* path, char const* tag);
    void                                    MarkAsTransformed(int32_t node);

    SceneNode&                              GetNodeAt(int32_t node);
    SceneTransformParameters&               GetTransformParamAt(int32_t node);
    char const*                             GetNameAt(int32_t node);

    void                                    Update();
private:
    void                                    RecalculateGlobalTransforms();
private:
    int32_t                                 ParseNode(int32_t parent_id, int32_t level, aiScene const* ai_scene, aiNode const* ai_node);
    [[nodiscard]] static AssimpMesh         ParseMesh(aiMesh const* ai_mesh);
    [[nodiscard]] static AssimpMaterial     ParseMaterial(ID3D11DeviceContext& context, aiMaterial const* ai_material, char const* base_path);
    using VertexData = std::pair<Microsoft::WRL::ComPtr<ID3D11Buffer>, Microsoft::WRL::ComPtr<ID3D11Buffer>>;
    [[nodiscard]] static VertexData         ParseVertexData(aiMesh const* ai_mesh, UINT& index_count);
private:
    int32_t                                 AddNode(int32_t parent_id, int32_t level, DirectX::XMMATRIX const& local_transform = DirectX::XMMatrixIdentity());
private:
    std::vector<AssimpMesh>                 _mesh{};
    std::vector<AssimpMaterial>             _material{};
    std::vector<SceneNode>                  _tree;
    std::vector<std::string>                _nodeNames{};
private:
    std::vector<SceneTransformParameters>   _transforms;
    std::vector<DirectX::XMFLOAT4X4>        _globalTransforms;
    std::vector<DirectX::XMFLOAT4X4>        _localTransforms;
public:
    std::unordered_map<uint32_t, uint32_t>  _nodeId_to_meshId{};
    std::unordered_map<uint32_t, uint32_t>  _nodeId_to_materialId{};
    std::unordered_map<uint32_t, uint32_t>  _nodeId_to_namesId{};
private:
    std::vector<int32_t>                    _transformed[MAX_NODE_LEVEL]{};
};

std::string   process_ai_path(char const* base_path, char const* ai_path);

