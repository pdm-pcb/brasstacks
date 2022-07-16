#ifndef BRASSTACKS_SHADERS_SHADER_HPP
#define BRASSTACKS_SHADERS_SHADER_HPP

#include <cstdint>

namespace btx {

class Shader {
public:
    static char * load_source(const char *path);

    virtual void bind()   const = 0;
    virtual void unbind() const = 0;

    virtual ~Shader() = default;

    Shader(const Shader &&) = delete;
    Shader(Shader &)        = delete;

    Shader & operator=(const Shader &&) = delete;
    Shader & operator=(Shader &)        = delete;

protected:
    Shader() = default;
};

} // namespace btx

#endif // BRASSTACKS_SHADERS_SHADER_HPP