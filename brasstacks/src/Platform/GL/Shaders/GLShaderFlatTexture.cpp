#include "brasstacks/System/pch.hpp"
#include "brasstacks/Platform/GL/Shaders/GLShaderFlatTexture.hpp"

#include "brasstacks/Shaders/Shader.hpp"

namespace btx {

void GLShaderFlatTexture::set_world(const glm::mat4 &world) const {
    glUniformMatrix4fv(_world_uniform, 1, GL_FALSE, &world[0][0]);
}

void GLShaderFlatTexture::_find_uniform() {
    _world_uniform = glGetUniformLocation(handle(), "world_matrix");
}

GLShaderFlatTexture::GLShaderFlatTexture() :
    GLShader(),
    _world_uniform { GL_NONE }
{
    add_program("../../assets/shaders/glsl/flat_texture.vert",
                Shader::Type::Vertex);
    add_program("../../assets/shaders/glsl/flat_texture.frag",
                Shader::Type::Pixel);

    link_programs();

    create_cam_ubo(handle());
    _find_uniform();
}

} // namespace btx