#ifndef BRASSTACKS_SHADERS_SHADER_HPP
#define BRASSTACKS_SHADERS_SHADER_HPP

#include "brasstacks/System/glm.hpp"

#include <cstdint>

namespace btx {

struct RenderComp;

class Shader {
public:
    struct CameraBufferData {
        glm::mat4 view;
        glm::mat4 projection;
    };

    static char * load_source(const char *path);

    virtual void bind()   const = 0;
    virtual void unbind() const = 0;
    
    virtual void update_camera(const glm::mat4 &view,
                               const glm::mat4 &projection) const = 0;

    virtual void update_render_data(const RenderComp &render_c) const = 0;

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