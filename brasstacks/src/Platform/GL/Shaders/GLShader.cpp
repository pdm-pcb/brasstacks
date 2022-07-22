#include "brasstacks/System/pch.hpp"
#include "brasstacks/Platform/GL/Shaders/GLShader.hpp"

#include "brasstacks/Shaders/Shader.hpp"
#include "brasstacks/Platform/GL/GLDebugger.hpp"

namespace btx {

GLuint GLShader::_cam_ubo = GL_NONE;

void GLShader::bind() {
    glUseProgram(_handle);
}

void GLShader::unbind() {
    glUseProgram(0);
}

void GLShader::update_camera(const glm::mat4 &view,
                             const glm::mat4 &projection) {
    Shader::CameraBufferData data { view, projection };

    glNamedBufferSubData(
        _cam_ubo, 0,
        sizeof(Shader::CameraBufferData),
        &data
    );
}

void GLShader::create_cam_ubo(GLuint handle) {
    static bool instantiated = false;
    if(instantiated) {
        return;
    }
    instantiated = true;

    glCreateBuffers(1, &_cam_ubo);
    assert(_cam_ubo != GL_NONE);
    glNamedBufferStorage(
        _cam_ubo,
        sizeof(Shader::CameraBufferData),
        nullptr,
        GL_DYNAMIC_STORAGE_BIT
    );
    
    GLuint index = glGetUniformBlockIndex(handle, "CameraMatrixBuffer");
    assert(index != GL_INVALID_INDEX);
    glBindBufferBase(GL_UNIFORM_BUFFER, index, _cam_ubo);
    BTX_ENGINE_TRACE("CameraMatrixBuffer index {}", index);
}

void GLShader::add_program(const char *path, const Shader::Type type) {
    char *source = Shader::load_source(path);

    switch(type) {
        case Shader::Type::Vertex:
        {
            _vert = glCreateShader(GL_VERTEX_SHADER);
            glShaderSource(_vert, 1, &source, nullptr);
            glCompileShader(_vert);
            
            GLint status;
            glGetShaderiv(_vert, GL_COMPILE_STATUS, &status);

            if(status == GL_FALSE) {
                glDeleteShader(_vert);
                BTX_ENGINE_ERROR("Faled to compile vertex shader.");
                _print_shader_log();
            }
            break;
        }

        case Shader::Type::Pixel:
        {
            _frag = glCreateShader(GL_FRAGMENT_SHADER);
            glShaderSource(_frag, 1, &source, nullptr);
            glCompileShader(_frag);
            
            GLint status;
            glGetShaderiv(_frag, GL_COMPILE_STATUS, &status);

            if(status == GL_FALSE) {
                glDeleteShader(_frag);
                BTX_ENGINE_ERROR("Faled to compile fragment shader.");
                _print_shader_log();
            }
            break;
        }

        case Shader::Type::Geometry:
        {
            _geo = glCreateShader(GL_GEOMETRY_SHADER);
            glShaderSource(_geo, 1, &source, nullptr);
            glCompileShader(_geo);
            
            GLint status;
            glGetShaderiv(_geo, GL_COMPILE_STATUS, &status);

            if(status == GL_FALSE) {
                glDeleteShader(_geo);
                BTX_ENGINE_ERROR("Faled to compile geometry shader.");
                _print_shader_log();
            }
            break;
        }

        default:
            BTX_ENGINE_WARN("Attempting to compile unsupported shader type.");
            break;
    }
    delete[] source;
}

void GLShader::link_programs() {
    if(_vert == GL_NONE || _frag == GL_NONE) {
        BTX_ENGINE_WARN("Cannot link shaders without vertex and pixel stages.");
    }

    glAttachShader(_handle, _vert);
    glAttachShader(_handle, _frag);

    if(_geo != GL_NONE) {
        glAttachShader(_handle, _geo);
    }

    glLinkProgram(_handle);

    glDeleteShader(_vert);
    glDeleteShader(_frag);
    glDeleteShader(_geo);

    GLint program_status;
    glGetProgramiv(_handle, GL_LINK_STATUS, &program_status);

    if(program_status == GL_FALSE) {
        glDeleteProgram(_handle);

        BTX_ENGINE_ERROR("Faled to link shaders.");
        _print_shader_log();
    }

    BTX_ENGINE_TRACE("Created shader program {}", _handle);
}

void GLShader::_print_shader_log() const {
    GLint log_length;
    glGetProgramiv(_handle, GL_INFO_LOG_LENGTH, &log_length);

    if(log_length < 0) {
        BTX_ENGINE_WARN("GL info log length returned {}", log_length);
        assert(false);
        return;
    }

    char *log = nullptr;
    log = new char[static_cast<std::size_t>(log_length)];

    if(log != nullptr) {
        glGetProgramInfoLog(_handle, log_length, nullptr, log);
        BTX_ENGINE_ERROR("OpenGL Shader Log:\n\t'{}'", log);
        delete[] log;
    }
}

GLShader::GLShader() :
    _handle  { GL_NONE },
    _vert    { GL_NONE },
    _frag    { GL_NONE },
    _geo     { GL_NONE }
{
    _handle = glCreateProgram();
    assert(_handle != GL_NONE);
}

GLShader::~GLShader() {
    glDeleteBuffers(1, &_cam_ubo);
    glDeleteProgram(_handle);
}

} // namespace pdr