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

    static void init() { }
    static void shutdown();

    static MeshIter const
    new_plane_mesh(float const scale,
                   std::array<Color const, 4> const &corner_colors = {
                       colors::white,
                       colors::white,
                       colors::white,
                       colors::white,
                   });

    static MeshIter const
    new_cube_mesh(float const scale,
                  std::array<Color const, 8> const &corner_colors = {
                       colors::white,
                       colors::white,
                       colors::white,
                       colors::white,
                       colors::white,
                       colors::white,
                       colors::white,
                       colors::white,
                  });

    static void unload_mesh(MeshIter const iter);

    MeshLibrary() = delete;
    ~MeshLibrary() = delete;

    MeshLibrary(MeshLibrary &&) = delete;
    MeshLibrary(MeshLibrary const &) = delete;

    MeshLibrary & operator=(MeshLibrary &&) = delete;
    MeshLibrary & operator=(MeshLibrary const &) = delete;

private:
    static MeshList _loaded_meshes;
};

} // namespace btx

#endif // BRASSTACKS_ASSETS_LIBRARIES_MESHLIBRARY_HPP