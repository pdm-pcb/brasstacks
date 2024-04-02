#include "brasstacks/brasstacks.hpp"
#include "brasstacks/assets/libraries/MeshLibrary.hpp"

#include "brasstacks/core/Renderer.hpp"

namespace btx {

MeshLibrary::MeshList MeshLibrary::_loaded_meshes { };

// =============================================================================
void MeshLibrary::shutdown() {
    _loaded_meshes.clear();
}

// =============================================================================
MeshLibrary::MeshIter const
MeshLibrary::new_plane_mesh(float const scale,
                            std::array<Color const, 4> const &corner_colors)
{
    _loaded_meshes.emplace_back(
        std::make_unique<PlaneMesh>(scale, corner_colors)
    );
    return --_loaded_meshes.end();
}

// =============================================================================
MeshLibrary::MeshIter const
MeshLibrary::new_cube_mesh(float const scale,
                           std::array<Color const, 8> const &corner_colors)
{
    _loaded_meshes.emplace_back(
        std::make_unique<CubeMesh>(scale, corner_colors)
    );
    return --_loaded_meshes.end();
}

// =============================================================================
void MeshLibrary::unload_mesh(MeshIter const iter) {
    _loaded_meshes.erase(iter);
}

} // namespace btx