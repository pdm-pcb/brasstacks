#ifndef BRASSTACKS_PLATFORM_GL_GLDEBUGGER_HPP
#define BRASSTACKS_PLATFORM_GL_GLDEBUGGER_HPP

#include "brasstacks/System/pch.hpp"

namespace btx {

class GLDebugger {
public:
	static void APIENTRY
    error_callback(GLenum source,
                   GLenum type, GLuint id, GLenum severity,
                   GLsizei length, const GLchar *message,
                   const void *user_data);

    static void check_error(const char *file, const int line);

	GLDebugger();
};

} // namespace btx

#endif // GLDBRASSTACKS_PLATFORM_GL_GLDEBUGGER_HPPEBUGGER_HPP