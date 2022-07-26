#ifndef BRASSTACKS_MESHES_MESHSCREENLOG_HPP
#define BRASSTACKS_MESHES_MESHSCREENLOG_HPP

#include "brasstacks/Meshes/Mesh.hpp"

namespace btx {

class MeshScreenLog : public Mesh {
public:
    typedef struct {
        glm::vec4 position;
    } Vertex;

    void bind_vertex_buffer() const override;
    void set_texture(Texture2D *diffuse) { _diffuse = diffuse; }
    void bind_texture() const;
    void set_buffer(const void *data, const std::size_t size);

    std::size_t index_count() const override { return _face_count * 3; }

    MeshScreenLog();
    ~MeshScreenLog();

    MeshScreenLog(const MeshScreenLog&) = delete;
    MeshScreenLog(MeshScreenLog&&)      = delete;
    MeshScreenLog& operator=(const MeshScreenLog&) = delete;
    MeshScreenLog& operator=(MeshScreenLog&&)      = delete;

private:
    VertexBuffer *_buffer;
    Vertex       _vertices[QUAD_VERTS];
    Face         _faces[QUAD_FACES];

    Texture2D  *_diffuse;

    std::size_t _vertex_count;
    std::size_t _face_count;
};

} // namespace btx

#endif // BRASSTACKS_MESHES_MESHScreenLog_HPP