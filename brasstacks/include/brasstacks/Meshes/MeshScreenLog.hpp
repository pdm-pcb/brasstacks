#ifndef BRASSTACKS_MESHES_MESHSCREENLOG_HPP
#define BRASSTACKS_MESHES_MESHSCREENLOG_HPP

#include "brasstacks/Meshes/Mesh.hpp"

namespace btx {

class MeshScreenLog : public Mesh {
public:
    typedef struct {
        glm::vec4 position;
        glm::vec2 texcoord;
    } Vertex;

    void bind_vertex_buffer() const override;
    void bind_texture() const;
    void update_buffer(const void *data, const std::size_t size,
                       const std::size_t offset = 0);

    void set_texture(Texture2D *diffuse) { _diffuse = diffuse; }
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
    Texture2D    *_diffuse;

    Vertex *_vertices;
    Face   *_faces;

    std::size_t _vertex_count;
    std::size_t _face_count;
};

} // namespace btx

#endif // BRASSTACKS_MESHES_MESHScreenLog_HPP