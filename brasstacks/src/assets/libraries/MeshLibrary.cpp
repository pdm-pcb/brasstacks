#include "brasstacks/brasstacks.hpp"
#include "brasstacks/assets/libraries/MeshLibrary.hpp"

#include "brasstacks/core/Renderer.hpp"

namespace btx {

// =============================================================================
MeshLibrary::MeshLibrary() :
    _loaded_meshes { }
{ }

// =============================================================================
MeshLibrary::~MeshLibrary() {
    for(auto iter : _loaded_meshes) {
        delete iter;
    }
    _loaded_meshes.clear();
}

// =============================================================================
MeshLibrary::MeshIter const
MeshLibrary::new_plane_mesh(std::span<Color const, 4> const corner_colors,
                            float const scale)
{
    _loaded_meshes.emplace_back(new PlaneMesh(corner_colors, scale));
    return --_loaded_meshes.end();
}

// =============================================================================
MeshLibrary::MeshIter const
MeshLibrary::new_cube_mesh(std::span<Color const, 8> const corner_colors,
                           float const scale)
{
    _loaded_meshes.emplace_back(new CubeMesh(corner_colors, scale));
    return --_loaded_meshes.end();
}

// =============================================================================
void MeshLibrary::unload_mesh(MeshIter const iter) {
    delete *iter;
    _loaded_meshes.erase(iter);
}

} // namespace btx