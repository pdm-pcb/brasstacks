#ifndef BRASSTACKS_ASSETS_LIBRARIES_MESHLIBRARY_HPP
#define BRASSTACKS_ASSETS_LIBRARIES_MESHLIBRARY_HPP

#include "brasstacks/pch.hpp"
#include "brasstacks/assets/meshes/PlaneMesh.hpp"
#include "brasstacks/assets/meshes/CubeMesh.hpp"
#include "brasstacks/math/colors.hpp"

namespace btx {

class MeshLibrary final {
public:
    using MeshList = std::list<Mesh *>;
    using MeshIter = MeshList::iterator;

    MeshLibrary();
    ~MeshLibrary();

    MeshIter const
    new_plane_mesh(std::span<Color const, 4> const corner_colors,
                   float const scale);

    MeshIter const
    new_cube_mesh(std::span<Color const, 8> const corner_colors,
                  float const scale);

    void unload_mesh(MeshIter const iter);

    MeshLibrary(MeshLibrary &&) = delete;
    MeshLibrary(MeshLibrary const &) = delete;

    MeshLibrary & operator=(MeshLibrary &&) = delete;
    MeshLibrary & operator=(MeshLibrary const &) = delete;

private:
    MeshList _loaded_meshes;
};

} // namespace btx

#endif // BRASSTACKS_ASSETS_LIBRARIES_MESHLIBRARY_HPP