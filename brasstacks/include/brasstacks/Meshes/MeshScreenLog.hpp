#ifndef BRASSTACKS_MESHES_MESHSCREENLOG_HPP
#define BRASSTACKS_MESHES_MESHSCREENLOG_HPP

#include "brasstacks/Meshes/Mesh.hpp"

#include <vector>

namespace btx {

class MeshScreenLog : public Mesh {
public:
    struct Vertex {
        glm::vec4 position;
        glm::vec2 texcoord;
    };

    struct MDICommand {
        ::GLuint index_count;
        ::GLuint instance_count;
        ::GLuint first_index;
        ::GLuint base_vertex;
        ::GLuint base_instance;
    };

    void bind_vertex_buffer() const override { }
    std::size_t index_count() const override { return 0; }

    void add_instance(Vertex vertices[4]);
    void create_commands();

    void set_texture(Texture2D *atlas) { _atlas = atlas; }

    MeshScreenLog();
    ~MeshScreenLog();

    MeshScreenLog(const MeshScreenLog&) = delete;
    MeshScreenLog(MeshScreenLog&&)      = delete;
    MeshScreenLog& operator=(const MeshScreenLog&) = delete;
    MeshScreenLog& operator=(MeshScreenLog&&)      = delete;

private:
    std::vector<Vertex>     _vertices;
    std::vector<MDICommand> _commands;

    VertexBuffer *_buffer;
    Face         *_faces;
    Texture2D    *_atlas;

    ::GLuint _indirect_buffer;
};

} // namespace btx

#endif // BRASSTACKS_MESHES_MESHScreenLog_HPP