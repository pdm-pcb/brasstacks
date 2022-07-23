#ifndef BRASSTACKS_ASSETLIBRARIES_SHADERLIBRARY_HPP
#define BRASSTACKS_ASSETLIBRARIES_SHADERLIBRARY_HPP

#include "brasstacks/Shaders/ShaderFlatColor.hpp"
#include "brasstacks/Shaders/ShaderFlatTexture.hpp"
#include "brasstacks/Shaders/ShaderLitTexture.hpp"

namespace btx {

class ShaderLibrary {
public:
    static void load(const char *vertex_filepath, const char *pixel_filepath,
                     const char *key);

    static Shader * checkout(const char *key);

    static void init();
    static void shutdown();
    static void remove(const char *key);

    ShaderLibrary() = delete;
    ~ShaderLibrary() = delete;

    ShaderLibrary(const ShaderLibrary &other) = delete;
    ShaderLibrary(ShaderLibrary &&other)      = delete;

    ShaderLibrary &operator=(const ShaderLibrary &other) = delete;
    ShaderLibrary &operator=(ShaderLibrary &&other)      = delete;

private:
    static std::unordered_map<std::string, Shader *> _shaders;
};

} // namespace btx

#endif // BRASSTACKS_ASSETLIBRARIES_SHADERLIBRARY_HPP