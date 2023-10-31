#include "pch.h"
#include "SceneGraph.h"

#include <queue>
#include <filesystem>

SceneGraph::SceneGraph(ID3D11DeviceContext& context, char const* path, char const* tag)
{
    unsigned constexpr flags{
		aiProcess_FlipUVs |
		aiProcess_GenUVCoords |
		aiProcess_GenNormals |
		aiProcess_CalcTangentSpace |
		aiProcess_Triangulate |
		aiProcess_JoinIdenticalVertices |
		aiProcess_ImproveCacheLocality
    };

    auto const ai_scene{
		aiImportFile(
			path,
			flags
		) 
    };

    if ((!ai_scene) | (!ai_scene->HasMeshes())) {
        exit(0);
    }

    // Parsing Meshes ============================================================================================
    _mesh.reserve(ai_scene->mNumMeshes);
    for (auto i{ 0 }; i != ai_scene->mNumMeshes; ++i)
        _mesh.push_back(ParseMesh(ai_scene->mMeshes[i]));
    // ===========================================================================================================

    // Parsing Materials =========================================================================================
    std::filesystem::path const fs_path{ path };
    std::string const base_path{ fs_path.parent_path().parent_path().string() + '/' };

    _material.reserve(ai_scene->mNumMaterials);
    for (auto i{ 0 }; i != ai_scene->mNumMaterials; ++i)
        _material.push_back(ParseMaterial(context, ai_scene->mMaterials[i], base_path.c_str()));
    // ===========================================================================================================

    // For the Root Node -------------------------
    auto const root_node{ ai_scene->mRootNode };
    ParseNode(-1, 0, ai_scene, root_node);

    _tree.reserve((root_node->mNumChildren + 1) * 3);

    std::queue<std::pair<int32_t, aiNode*>> bfs_queue;
    for (auto i{ 0 }; i != root_node->mNumChildren; ++i)
        bfs_queue.emplace(0, root_node->mChildren[i]);
    // -------------------------------------------

    // For the children --------------------------
    while (!bfs_queue.empty()) {
        auto const [parent_id, current_node] {bfs_queue.front()};
        bfs_queue.pop();

        auto const cur_id{ ParseNode(parent_id, _tree[parent_id]._level + 1, ai_scene, current_node) };

        for (auto i{ 0 }; i != current_node->mNumChildren; ++i)
            bfs_queue.emplace(cur_id, current_node->mChildren[i]);
    }
    // -------------------------------------------

    _tree.shrink_to_fit();
    _nodeNames[0] = { tag };
    MarkAsTransformed(0);
    RecalculateGlobalTransforms();
}

void SceneGraph::MarkAsTransformed(int32_t node)
{
    auto const level{ _tree[node]._level };
    _transformed[level].push_back(node);

    for (int32_t s{ _tree[node]._firstChild }; s != -1; s = _tree[s]._nextSibling)
        MarkAsTransformed(s);
}

SceneNode& SceneGraph::GetNodeAt(int32_t node)
{
    return _tree[node];
}

SceneTransformParameters& SceneGraph::GetTransformParamAt(int32_t node)
{
    return _transforms[node];
}

char const* SceneGraph::GetNameAt(int32_t node)
{
    return _nodeNames[_nodeId_to_namesId.at(node)].c_str();
}

void SceneGraph::Update()
{
    RecalculateGlobalTransforms();
}

void SceneGraph::RecalculateGlobalTransforms()
{
    using namespace DirectX;

    // For the Root Layer (Their global transforms are always coincide with local transforms)
    if (!_transformed[0].empty()) {
        auto const cur{ _transformed[0][0] };

        XMStoreFloat4x4(
            &_globalTransforms[cur],
            XMLoadFloat4x4(&_localTransforms[cur]) *
            XMMatrixMultiply(
                XMMatrixScaling(_transforms[cur].scale, _transforms[cur].scale, _transforms[cur].scale),
                XMMatrixMultiply(
                    XMMatrixRotationRollPitchYaw(_transforms[cur].pitch, _transforms[cur].yaw, _transforms[cur].roll),
                    XMMatrixTranslation(_transforms[cur].x, _transforms[cur].y, _transforms[cur].z)
                )
            )
        );

        _transformed[0].clear();
    }
    // --------------------------------------------------------------------------------------


    // Iterate for each nodes (all of the nodes have parents)
    for (int32_t i{ 1 }; i != MAX_NODE_LEVEL && !_transformed[i].empty(); ++i) {
        for (auto const cur : _transformed[i]) {
            auto const parent{ _tree[cur]._parent };

            XMStoreFloat4x4(
                &_globalTransforms[cur],
                XMMatrixMultiply(
                    XMLoadFloat4x4(&_globalTransforms[parent]),
                    XMLoadFloat4x4(&_localTransforms[cur]) *
                    XMMatrixMultiply(
                        XMMatrixScaling(_transforms[cur].scale, _transforms[cur].scale, _transforms[cur].scale),
                        XMMatrixMultiply(
                            XMMatrixRotationRollPitchYaw(_transforms[cur].pitch, _transforms[cur].yaw, _transforms[cur].roll),
                            XMMatrixTranslation(_transforms[cur].x, _transforms[cur].y, _transforms[cur].z)
                        )
                    )
                )
            );
        }

        _transformed[i].clear();
    }
}

int32_t SceneGraph::ParseNode(int32_t parent_id, int32_t level, aiScene const* ai_scene, aiNode const* ai_node)
{
    auto const local_transform{
    DirectX::XMMatrixTranspose(
        DirectX::XMLoadFloat4x4(
            reinterpret_cast<DirectX::XMFLOAT4X4 const*>(&ai_node->mTransformation)
        )
    )
    };
    auto const id{ AddNode(parent_id, level, local_transform) };

    if (ai_node->mName.C_Str()) {
        uint32_t const name_id{ static_cast<uint32_t>(_nodeNames.size()) };
        _nodeNames.emplace_back(ai_node->mName.C_Str());
        _nodeId_to_namesId[id] = name_id;
    }

    for (auto i{ 0 }; i != ai_node->mNumMeshes; ++i) {
        auto const sub_node_id{ AddNode(id, level + 1) };
        if (_tree[id]._firstChild == -1)
            _tree[id]._firstChild = sub_node_id;

        std::string const parent_node_name{ ai_node->mName.C_Str() };
        _nodeId_to_namesId[sub_node_id] = _nodeNames.size();
        _nodeNames.emplace_back(parent_node_name + "-mesh" + std::to_string(i));
        _nodeId_to_meshId[sub_node_id] = ai_node->mMeshes[i];
        _nodeId_to_materialId[sub_node_id] = ai_scene->mMeshes[ai_node->mMeshes[i]]->mMaterialIndex;
    }

    return id;
}

AssimpMesh SceneGraph::ParseMesh(aiMesh const* ai_mesh)
{
    std::string vertex_attribute{};
    if (ai_mesh->HasPositions())
        vertex_attribute += "1p";
    if (ai_mesh->HasNormals())
        vertex_attribute += "1n";
    if (ai_mesh->HasTangentsAndBitangents())
        vertex_attribute += "1t";
    if (ai_mesh->HasTextureCoords(0u))
        vertex_attribute += "1uv";

    auto [vertex_buffer, index_buffer] {ParseVertexData(ai_mesh, vertex_attribute)};

    return {
        vertex_buffer,
        index_buffer,
        D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST,
    };
}

AssimpMaterial SceneGraph::ParseMaterial(ID3D11DeviceContext& context, aiMaterial const* ai_material, char const* base_path)
{
    AssimpMaterial result{};

    // Colors ------------------------------------------------------------------------------------------------
    aiColor4D color;
    if (aiGetMaterialColor(ai_material, AI_MATKEY_COLOR_AMBIENT, &color) == AI_SUCCESS) {
        result._emissiveColor = DirectX::XMFLOAT4{ color.r, color.g, color.b, color.a };
        result._emissiveColor.w = std::clamp(result._emissiveColor.w, 0.0f, 1.0f);
    }

    if (aiGetMaterialColor(ai_material, AI_MATKEY_COLOR_DIFFUSE, &color) == AI_SUCCESS) {
        result._albedoColor = DirectX::XMFLOAT4{ color.r, color.g, color.b, color.a };
        result._albedoColor.w = std::clamp(result._emissiveColor.w, 0.0f, 1.0f);
    }

    if (aiGetMaterialColor(ai_material, AI_MATKEY_COLOR_EMISSIVE, &color) == AI_SUCCESS) {
        result._emissiveColor.x += color.r;
        result._emissiveColor.y += color.g;
        result._emissiveColor.z += color.b;
        result._emissiveColor.w += color.a;
        result._emissiveColor.w = std::clamp(result._emissiveColor.w, 0.0f, 1.0f);
    }
    // -------------------------------------------------------------------------------------------------------

    // Transparency ------------------------------------------------------------------------------------------
    float constexpr opaquenessThreshold{ 0.05f };
    float opacity{};
    if (aiGetMaterialFloat(ai_material, AI_MATKEY_OPACITY, &opacity) == AI_SUCCESS) {
        result._transparencyFactor = std::clamp(1.0f - opacity, 0.0f, 1.0f);
        if (result._transparencyFactor >= 1.0f - opaquenessThreshold)
            result._transparencyFactor = 0.0f;
    }

    if (aiGetMaterialColor(ai_material, AI_MATKEY_COLOR_TRANSPARENT, &color) == AI_SUCCESS) {
        float const temp_opacity{ std::max(std::max(color.r, color.g), color.b) };
        result._transparencyFactor = std::clamp(temp_opacity, 0.0f, 1.0f);
        if (result._transparencyFactor >= 1.0f - opaquenessThreshold)
            result._transparencyFactor = 0.0f;
        result._alphaTest = 0.5f;
    }
    // -------------------------------------------------------------------------------------------------------

    // Metallic Factor ----------------------------------------------------------------------------------------
    {
        float temp{ 1.0f };
        if (aiGetMaterialFloat(ai_material, AI_MATKEY_COLOR_TRANSPARENT, &temp) == AI_SUCCESS)
            result._metallicFactor = temp;
    }
    // -------------------------------------------------------------------------------------------------------

    // Roughness Factor --------------------------------------------------------------------------------------
    {
        float temp{ 1.0f };
        if (aiGetMaterialFloat(ai_material, AI_MATKEY_ROUGHNESS_FACTOR, &temp) == AI_SUCCESS)
            result._roughness = DirectX::XMFLOAT4{ temp, temp, temp, temp };
    }
    // -------------------------------------------------------------------------------------------------------

    // Textures
    aiString ai_path;
    aiTextureMapping mapping;
    uint32_t uv_index{};
    float blend{ 1.0f };
    aiTextureOp texture_op{ aiTextureOp_Add };
    aiTextureMapMode texture_map_mode[2]{ aiTextureMapMode_Wrap, aiTextureMapMode_Wrap };
    uint32_t texture_flags{ 0u };

    // Emissive Map
    if (aiGetMaterialTexture(ai_material, aiTextureType_EMISSIVE, 0u, &ai_path, &mapping, &uv_index, &blend, &texture_op, texture_map_mode, &texture_flags) == AI_SUCCESS) {
        auto const final_path{ process_ai_path(base_path, ai_path.C_Str()) };

        result.AddOrRelplaceTexture(context, ShaderResourceTypes::EmissiveMap, final_path.c_str());
    }

    // Diffuse Map
    if (aiGetMaterialTexture(ai_material, aiTextureType_DIFFUSE, 0u, &ai_path, &mapping, &uv_index, &blend, &texture_op, texture_map_mode, &texture_flags) == AI_SUCCESS) {
        auto const final_path{ process_ai_path(base_path, ai_path.C_Str()) };

        result.AddOrRelplaceTexture(context, ShaderResourceTypes::DiffuseMap, final_path.c_str());
    }

    // Specular Map
    if (aiGetMaterialTexture(ai_material, aiTextureType_SPECULAR, 0u, &ai_path, &mapping, &uv_index, &blend, &texture_op, texture_map_mode, &texture_flags) == AI_SUCCESS) {
        auto const final_path{ process_ai_path(base_path, ai_path.C_Str()) };

        result.AddOrRelplaceTexture(context, ShaderResourceTypes::SpecularMap, final_path.c_str());
    }

    // Normal Map
    if (aiGetMaterialTexture(ai_material, aiTextureType_NORMALS, 0u, &ai_path, &mapping, &uv_index, &blend, &texture_op, texture_map_mode, &texture_flags) == AI_SUCCESS) {
        auto const final_path{ process_ai_path(base_path, ai_path.C_Str()) };

        result.AddOrRelplaceTexture(context, ShaderResourceTypes::NormalMap, final_path.c_str());
    }

    // Height Map
    if (aiGetMaterialTexture(ai_material, aiTextureType_HEIGHT, 0u, &ai_path, &mapping, &uv_index, &blend, &texture_op, texture_map_mode, &texture_flags) == AI_SUCCESS) {
        auto const final_path{ process_ai_path(base_path, ai_path.C_Str()) };

        result.AddOrRelplaceTexture(context, ShaderResourceTypes::HeightMap, final_path.c_str());
    }

    // Metallic
    if (aiGetMaterialTexture(ai_material, aiTextureType_METALNESS, 0u, &ai_path, &mapping, &uv_index, &blend, &texture_op, texture_map_mode, &texture_flags) == AI_SUCCESS) {
        auto const final_path{ process_ai_path(base_path, ai_path.C_Str()) };

        result.AddOrRelplaceTexture(context, ShaderResourceTypes::MetallicMap, final_path.c_str());
    }

    // Roughness
    if (aiGetMaterialTexture(ai_material, aiTextureType_DIFFUSE_ROUGHNESS, 0u, &ai_path, &mapping, &uv_index, &blend, &texture_op, texture_map_mode, &texture_flags) == AI_SUCCESS) {
        auto const final_path{ process_ai_path(base_path, ai_path.C_Str()) };

        result.AddOrRelplaceTexture(context, ShaderResourceTypes::RoughnessMap, final_path.c_str());
    }

    // AO
    if (aiGetMaterialTexture(ai_material, aiTextureType_AMBIENT_OCCLUSION, 0u, &ai_path, &mapping, &uv_index, &blend, &texture_op, texture_map_mode, &texture_flags) == AI_SUCCESS) {
        auto const final_path{ process_ai_path(base_path, ai_path.C_Str()) };

        result.AddOrRelplaceTexture(context, ShaderResourceTypes::AOMap, final_path.c_str());
    }

    //Opacity Map
    if (aiGetMaterialTexture(ai_material, aiTextureType_OPACITY, 0u, &ai_path, &mapping, &uv_index, &blend, &texture_op, texture_map_mode, &texture_flags) == AI_SUCCESS) {
        auto const final_path {process_ai_path(base_path, ai_path.C_Str())};

        result.AddOrRelplaceTexture(context, ShaderResourceTypes::OpacityMap, final_path.c_str());
    }

    return result;
}

std::pair<Microsoft::WRL::ComPtr<ID3D11Buffer>, Microsoft::WRL::ComPtr<ID3D11Buffer>> SceneGraph::ParseVertexData(aiMesh const* ai_mesh)
{
    auto const buffer{ D3D11VertexAttribute::GetBuffer(ai_mesh, vertex_format) };

    std::vector<uint32_t> indices;
    indices.reserve(ai_mesh->mNumFaces * 3);
    for (auto i{ 0 }; i != ai_mesh->mNumFaces; ++i) {
        auto const& face{ ai_mesh->mFaces[i] };
        indices.push_back(face.mIndices[0]);
        indices.push_back(face.mIndices[1]);
        indices.push_back(face.mIndices[2]);
    }

    auto vertex_buffer{ D3D11MeshDataHolder::ResolveVertexBuffer(device, D3D11VertexAttribute::GetStride(vertex_format), buffer.size(), buffer.data(), ai_mesh->mName.C_Str()) };
    auto index_buffer{ D3D11MeshDataHolder::ResolveIndexBuffer(device, indices.data(), indices.size(), ai_mesh->mName.C_Str()) };

    return { vertex_buffer, index_buffer };
}

int32_t SceneGraph::AddNode(int32_t parent_id, int32_t level, DirectX::XMMATRIX const& local_transform)
{
    int32_t const id{ static_cast<int32_t>(_tree.size()) };
    {
        _transforms.emplace_back();
        _globalTransforms.emplace_back();
        _localTransforms.emplace_back();
        DirectX::XMStoreFloat4x4(&_globalTransforms.back(), DirectX::XMMatrixIdentity());
        DirectX::XMStoreFloat4x4(&_localTransforms.back(), local_transform);
    }

    _tree.emplace_back(parent_id, -1, -1, -1, level);
    if (parent_id > -1) {
        // get first sibling to find the position
        auto s{ _tree[parent_id]._firstChild };
        if (s == -1) { // there is no sibling
            _tree[parent_id]._firstChild = id;
            _tree[id]._lastSibling = id;
        }
        else { // there is
            auto pos = _tree[s]._lastSibling;
            if (pos <= -1) {
                for (pos = s; _tree[pos]._nextSibling != -1; pos = _tree[pos]._nextSibling)
                    ;
            }
            _tree[pos]._nextSibling = id;
            _tree[s]._lastSibling = id;
        }
    }
    _tree[id]._level = level;
    _tree[id]._nextSibling = -1;
    _tree[id]._firstChild = -1;

    return id;
}

std::string process_ai_path(char const* base_path, char const* ai_path)
{
    std::filesystem::path temp{ ai_path };

    if (temp.is_relative()) {
        auto pos{ size_t{} };
        auto s{ temp.string() };
        while (s[pos] == '.')
            ++pos;
        s = s.substr(pos);
        temp = std::filesystem::path{ s };
    }

    temp = std::filesystem::path{ std::string{"Textures/"} + temp.filename().string() };

    temp.replace_extension("dds");

    std::filesystem::path result{ base_path };
    result.concat(temp.string());
    result.make_preferred();

    return result.string();
}

void AssimpMaterial::AddOrRelplaceTexture(ID3D11DeviceContext& context, ShaderResourceTypes type, char const* path)
{
    auto const target_id{ static_cast<int>(type) };
    if (_textureTypes.test(target_id)) {
        for (auto& sr : _srs) {
            if (auto const type_id{ std::visit(GetShaderResourceTypeID{}, sr) }; type_id == target_id) {
                std::swap(sr, _srs.back());
                _srs.pop_back();
                break;
            }
        }
    }
    else {
        _textureTypes.set(target_id);
    }
    _srs.push_back(D3D11ShaderResourceHolder::Resolve(device, context, type, path));
}

AssimpMesh::AssimpMesh(Microsoft::WRL::ComPtr<ID3D11Buffer>&& vertex_buffer, Microsoft::WRL::ComPtr<ID3D11Buffer>&& index_buffer, D3D11_PRIMITIVE_TOPOLOGY topology)
    : _vertexBuffer{std::move(vertex_buffer)}, _indexBuffer{std::move(index_buffer)}, _topology{topology}
{
}

void AssimpMesh::Bind(ID3D11DeviceContext& context) const
{
    context.IASetPrimitiveTopology(_topology);
    context.IASetVertexBuffers(0u, 1u, _vertexBuffer.GetAddressOf(), 0u, 0u);
    context.IASetIndexBuffer(_indexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0u);
}

UINT AssimpMesh::GetIndexCount() const
{
    return _indexCount;
}
