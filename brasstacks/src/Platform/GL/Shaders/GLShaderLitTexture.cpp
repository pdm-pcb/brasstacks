#include "brasstacks/System/pch.hpp"
#include "brasstacks/Platform/GL/Shaders/GLShaderLitTexture.hpp"

namespace btx {

void GLShaderLitTexture::set_world_and_material(
    const Shader::WorldAndMaterial &params)
{
    glNamedBufferSubData(
        _world_material_ubo, 0,
        sizeof(Shader::WorldAndMaterial),
        &params
    );
}

void GLShaderLitTexture::set_light_params(const Shader::LightParameters &params)
{
    glNamedBufferSubData(
        _light_param_ubo, 0,
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
    assert(index != GL_INVALID_INDEX);
    glBindBufferBase(GL_UNIFORM_BUFFER, index, _world_material_ubo);
    BTX_ENGINE_TRACE("WorldAndMaterial index {}", index);

    glCreateBuffers(1, &_light_param_ubo);
    glNamedBufferStorage(
        _light_param_ubo,
        sizeof(Shader::LightParameters),
        nullptr,
        GL_DYNAMIC_STORAGE_BIT
    );
    
    index = glGetUniformBlockIndex(handle(), "LightParameters");
    assert(index != GL_INVALID_INDEX);
    glBindBufferBase(GL_UNIFORM_BUFFER, index, _light_param_ubo);
    BTX_ENGINE_TRACE("LightParameters index {}", index);

    assert(_world_material_ubo != GL_NONE);
    assert(_light_param_ubo != GL_NONE);
}

GLShaderLitTexture::GLShaderLitTexture() :
    GLShader(),
    _world_material_ubo { GL_NONE },
    _light_param_ubo    { GL_NONE }
{
    BTX_ENGINE_INFO("GLShaderLitTexture vertex shader");
    add_program("../../assets/shaders/glsl/lit_texture.vert",
                Shader::Type::Vertex);

    BTX_ENGINE_INFO("GLShaderLitTexture pixel shader");
    add_program("../../assets/shaders/glsl/lit_texture.frag",
                Shader::Type::Pixel);
    link_programs();

    create_cam_ubo(handle());
    _create_ubos();
}

} // namespace btx