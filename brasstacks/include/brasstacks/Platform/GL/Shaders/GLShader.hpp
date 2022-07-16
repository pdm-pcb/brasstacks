#ifndef BRASSTACKS_PLATFORM_GL_SHADERS_GLSHADER_HPP
#define BRASSTACKS_PLATFORM_GL_SHADERS_GLSHADER_HPP

namespace btx {

class GLShader {
public:
    enum class Type {
        Vertex,
        Fragment,
        Geometry
    };

    void bind();
    static void unbind();

    void add_program(const char *path, const Type type);
    void link_program();

    GLShader();
    ~GLShader();

    GLShader(const GLShader &other)             = delete;
    GLShader(GLShader &&other)                  = delete;

    GLShader & operator=(const GLShader &other) = delete;
    GLShader & operator=(GLShader &&other)      = delete;

protected:
    GLuint handle() const { return _handle; }

private:
    GLuint _handle;
    GLuint _vert;
    GLuint _frag;
    GLuint _geo;

    void _print_shader_log()  const;    
};

} // namespace btx

#endif // BRASSTACKS_PLATFORM_GL_SHADERS_GLSHADER_HPP