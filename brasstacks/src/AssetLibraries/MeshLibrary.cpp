#include "brasstacks/System/pch.hpp"
#include "brasstacks/AssetLibraries/MeshLibrary.hpp"

namespace btx {

std::unordered_map<std::string, Mesh *> MeshLibrary::_meshes;

void MeshLibrary::load(Mesh::Type type, const char *key,
                       const Mesh::Primitives primitive,
                       const glm::vec3 &color,
                       const float u_repeat, const float v_repeat,
                       const float scale, const float plane_offset)
{
    if(_meshes.find(key) != _meshes.end()) {
        BTX_ENGINE_WARN("Mesh '{}' already exists", key);
        assert(false);
        return;
    }

    Mesh *mesh = nullptr; 
    
    switch(type) {
        case Mesh::Type::FlatColor:
            mesh = new MeshFlatColor(
                primitive,
                color,
                scale,
                plane_offset
            );
            break;

        case Mesh::Type::FlatTexture:
            mesh = new MeshFlatTexture(
                primitive, u_repeat, v_repeat,
                scale, plane_offset
            );
            break;

        case Mesh::Type::LitTexture:
            mesh = new MeshLitTexture(
                primitive, u_repeat, v_repeat,
                scale, plane_offset
            );
            break;

        default:
            BTX_ENGINE_WARN("Unsupported mesh type primitive.");
            assert(false);
            break;
    }
    
    _meshes.insert({ key, mesh });
}

Mesh * MeshLibrary::checkout(const char *key) {
    auto texture = _meshes.find(key);
    if(texture == _meshes.end()) {
        BTX_ENGINE_WARN("Cannot find mesh '{}'", key);
        assert(false);
        return nullptr;
    }

    return texture->second;
}

void MeshLibrary::init() {
    MeshLibrary::load(Mesh::Type::LitTexture,
                      "lit_texture_cube",
                      Mesh::Primitives::Cube);

    MeshLibrary::load(Mesh::Type::FlatColor,
                      "flat_color_cube_white",
                      Mesh::Primitives::Cube);

    MeshLibrary::load(
        Mesh::Type::FlatColor,
        "flat_color_cube_red",
        Mesh::Primitives::Cube,
        { 1.0f, 0.0f, 0.0f }
    );

    MeshLibrary::load(
        Mesh::Type::FlatColor,
        "flat_color_cube_green",
        Mesh::Primitives::Cube,
        { 0.0f, 1.0f, 0.0f }
    );

    MeshLibrary::load(
        Mesh::Type::FlatColor,
        "flat_color_cube_blue",
        Mesh::Primitives::Cube,
        { 0.0f, 0.0f, 1.0f }
    );
}

void MeshLibrary::shutdown() {
    for(auto &texture : _meshes) {
        delete texture.second;
    }

    _meshes.clear();
}

void MeshLibrary::remove(const char *key) {
    auto texture = _meshes.find(key);
    if(texture == _meshes.end()) {
        BTX_ENGINE_WARN("Cannot find mesh '{}'", key);
        assert(false);
        return;
    }

    delete texture->second;

    _meshes.erase(texture);
}

} // namespace btx