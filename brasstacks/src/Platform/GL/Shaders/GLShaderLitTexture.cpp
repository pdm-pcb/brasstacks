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
    assert(_world_material_ubo != GL_NONE);
    glNamedBufferStorage(
        _world_material_ubo,
        sizeof(Shader::WorldAndMaterial),
        nullptr,
        GL_DYNAMIC_STORAGE_BIT
    );
    glBindBufferBase(GL_UNIFORM_BUFFER, 1, _world_material_ubo);

    glCreateBuffers(1, &_light_param_ubo);
    assert(_light_param_ubo != GL_NONE);
    glNamedBufferStorage(
        _light_param_ubo,
        sizeof(Shader::LightParameters),
        nullptr,
        GL_DYNAMIC_STORAGE_BIT
    );
    glBindBufferBase(GL_UNIFORM_BUFFER, 2, _light_param_ubo);
}

GLShaderLitTexture::GLShaderLitTexture() :
    GLShader(),
    _world_material_ubo { GL_NONE },
    _light_param_ubo    { GL_NONE },
    _world_binding { 3 },
    _light_binding { 4 }
{
    BTX_ENGINE_INFO("GLShaderLitTexture vertex shader");
    add_program("../../assets/shaders/glsl/lit_texture.vert",
                Shader::Type::Vertex);

    BTX_ENGINE_INFO("GLShaderLitTexture pixel shader");
    add_program("../../assets/shaders/glsl/lit_texture.frag",
                Shader::Type::Pixel);
    link_programs();

    create_cam_ubo();
    _create_ubos();
}

} // namespace btx