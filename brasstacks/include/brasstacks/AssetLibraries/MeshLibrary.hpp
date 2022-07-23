#ifndef BRASSTACKS_ASSETLIBRARIES_MESHLIBRARY_HPP
#define BRASSTACKS_ASSETLIBRARIES_MESHLIBRARY_HPP

#include "brasstacks/Meshes/MeshFlatColor.hpp"
#include "brasstacks/Meshes/MeshFlatTexture.hpp"
#include "brasstacks/Meshes/MeshLitTexture.hpp"

#include "unordered_map"
#include "string"

namespace btx {

class MeshLibrary final {
public:
    static void load(Mesh::Type type, const char *key,
                     const Mesh::Primitives primitive,
                     const glm::vec3 &color = { 1.0f, 1.0f, 1.0f },
                     const float u_repeat = 1.0f,
                     const float v_repeat = 1.0f,
                     const float scale = 1.0f,
                     const float plane_offset = 0.0f);

    static Mesh * checkout(const char *key);

    static void init();
    static void shutdown();
    static void remove(const char *key);

    MeshLibrary() = delete;
    ~MeshLibrary() = delete;

    MeshLibrary(const MeshLibrary &other) = delete;
    MeshLibrary(MeshLibrary &&other)      = delete;

    MeshLibrary &operator=(const MeshLibrary &other) = delete;
    MeshLibrary &operator=(MeshLibrary &&other)      = delete;

private:
    static std::unordered_map<std::string, Mesh *> _meshes;
};

} // namespace btx

#endif // BRASSTACKS_ASSETLIBRARIES_MESHLIBRARY_HPP