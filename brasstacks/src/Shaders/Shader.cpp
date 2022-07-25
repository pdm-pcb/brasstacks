#include "brasstacks/System/pch.hpp"
#include "brasstacks/Shaders/Shader.hpp"

namespace btx {

char *Shader::load_source(const char *filepath) {
    if(filepath == nullptr) {
        BTX_ENGINE_ERROR("No file name provided to load_source()");
        assert(false);
        return nullptr;
    }

    std::ifstream input_file(filepath);

    if(!input_file.good()) {
        BTX_ENGINE_ERROR("Unable to open source '{}'", filepath);
        assert(false);
        return nullptr;
    }

    input_file.seekg(0, input_file.end);
    auto filesize = input_file.tellg();
    input_file.seekg(0, input_file.beg);

    char *dest = new char[static_cast<std::size_t>(filesize) + 1] { };
    memset(dest, 0, static_cast<std::size_t>(filesize)  + 1);

    input_file.read(dest, filesize);
    input_file.close();

    BTX_ENGINE_TRACE("Loaded Shader source {}", filepath);

    return dest;
}

} // namespace btx