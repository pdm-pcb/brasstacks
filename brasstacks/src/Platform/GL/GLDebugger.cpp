#include "brasstacks/System/pch.hpp"
#include "brasstacks/Platform/GL/GLDebugger.hpp"

#include <filesystem>

namespace btx {

#ifdef DEBUG

void APIENTRY
GLDebugger::error_callback(GLenum source,
                           GLenum type, GLuint id, GLenum severity,
          [[maybe_unused]] GLsizei length, const GLchar *message,
          [[maybe_unused]] const void *user_data) {

    const char *_severity;
    const char *_source;
    const char *_type;

    switch(severity) {
        case GL_DEBUG_SEVERITY_HIGH:         _severity = "High";    break;
        case GL_DEBUG_SEVERITY_MEDIUM:       _severity = "Medium";  break;
        case GL_DEBUG_SEVERITY_LOW:          _severity = "Low";     break;
        case GL_DEBUG_SEVERITY_NOTIFICATION: _severity = "Note";    break;
        default:                             _severity = "Unknown"; break;
    }

    switch(source) {
        case GL_DEBUG_SOURCE_API:             _source = "API";            break;
        case GL_DEBUG_SOURCE_WINDOW_SYSTEM:   _source = "Window System";  break;
        case GL_DEBUG_SOURCE_SHADER_COMPILER: _source = "Shader Compile"; break;
        case GL_DEBUG_SOURCE_THIRD_PARTY:     _source = "Third Party";    break;
        case GL_DEBUG_SOURCE_APPLICATION:     _source = "Application";    break;
        case GL_DEBUG_SOURCE_OTHER:           _source = "Other";          break;
        default:                              _source = "Unknown";        break;
    }

    switch(type) {
        case GL_DEBUG_TYPE_ERROR:               _type = "Error";       break;
        case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR: _type = "Depricated";  break;
        case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:  _type = "Undefined";   break;
        case GL_DEBUG_TYPE_PORTABILITY:         _type = "Portability"; break;
        case GL_DEBUG_TYPE_PERFORMANCE:         _type = "Performance"; break;
        case GL_DEBUG_TYPE_OTHER:               _type = "Other";       break;
        case GL_DEBUG_TYPE_MARKER:              _type = "Marker";      break;
        default:                                _type = "Unknown";     break;
    }

    if(type == GL_DEBUG_TYPE_ERROR) {
        BTX_ENGINE_ERROR("[GL {} #{}] {} from {}:\n     {}",
                         _type, id, _severity, _source, message);
        assert(false);
    }
    else if (severity == GL_DEBUG_SEVERITY_HIGH ||
             severity == GL_DEBUG_SEVERITY_MEDIUM){
        BTX_ENGINE_WARN("[GL {} #{}] {} from {}:\n     {}",
                        _type, id, _severity, _source, message);
    }
    else {
        BTX_ENGINE_TRACE("[GL {} #{}] {} from {}:\n     {}",
                        _type, id, _severity, _source, message);
    }
}

GLDebugger::GLDebugger() {
    glEnable(GL_DEBUG_OUTPUT);
    glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
    glDebugMessageCallback(&error_callback, nullptr);
    glDebugMessageControl(
        GL_DONT_CARE,       // Message source
        GL_DONT_CARE,       // Message type
        GL_DONT_CARE,       // Message severity
        0,                  // User data array size
        nullptr,            // User data array
        GL_TRUE             // Enable messages corresponding to the above
    );                      // filters

    BTX_ENGINE_TRACE("OpenGL debugger online");
}

#endif // DEBUG

} // namespace btx