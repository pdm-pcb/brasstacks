#include "brasstacks/System/pch.hpp"
#include "brasstacks/Platform/GL/Shaders/GLShaderLitTexture.hpp"

namespace btx {

void GLShaderLitTexture::set_world_and_material(
    const Shader::WorldAndMaterial &params)
{
    glNamedBufferSubData(
        _world_material_ubo,
        0,
        sizeof(Shader::WorldAndMaterial),
        &params
    );
}

void GLShaderLitTexture::set_light_params(const Shader::LightParameters &params)
{
    glNamedBufferSubData(
        _light_param_ubo,
        0,
        sizeof(Shader::LightParameters),
        &params
    );
}

void GLShaderLitTexture::_create_ubos() {
    glCreateBuffers(1, &_world_material_ubo);
    glNamedBufferStorage(
        _world_material_ubo,
        sizeof(Shader::WorldAndMaterial),
        nullptr,
        GL_DYNAMIC_STORAGE_BIT
    );
    
    GLuint index = glGetUniformBlockIndex(handle(), "WorldAndMaterial");
    glBindBufferBase(GL_UNIFORM_BUFFER, index, _world_material_ubo);

    glCreateBuffers(1, &_light_param_ubo);
    glNamedBufferStorage(
        _light_param_ubo,
        sizeof(Shader::LightParameters),
        nullptr,
        GL_DYNAMIC_STORAGE_BIT
    );
    
    GLuint index = glGetUniformBlockIndex(handle(), "LightParameters");
    glBindBufferBase(GL_UNIFORM_BUFFER, index, _light_param_ubo);
}

GLShaderLitTexture::GLShaderLitTexture() :
    _world_material_ubo { GL_NONE }
{
    add_program("../../assets/shaders/glsl/lit_texture.vert",
                Shader::Type::Vertex);
    add_program("../../assets/shaders/glsl/lit_texture.frag",
                Shader::Type::Fragment);
    link_program();

    _create_ubos();
    _find_uniforms();
}

} // namespace btx