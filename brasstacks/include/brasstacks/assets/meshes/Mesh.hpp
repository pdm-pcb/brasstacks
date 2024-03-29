#ifndef BRASSTACKS_ASSETS_MESHES_MESH_HPP
#define BRASSTACKS_ASSETS_MESHES_MESH_HPP

#include "brasstacks/pch.hpp"
#include "brasstacks/assets/meshes/Vertex.hpp"
#include "brasstacks/assets/meshes/Index.hpp"

namespace btx {

class vkDevice;
class vkBuffer;
class vkCmdBuffer;

class Mesh {
public:
    void draw_indexed(vkCmdBuffer const &cmd_buffer) const;

    virtual ~Mesh();

    Mesh(Mesh &&) = delete;
    Mesh(const Mesh &) = delete;

    Mesh& operator=(Mesh &&) = delete;
    Mesh& operator=(Mesh const &) = delete;

protected:
    explicit Mesh(vkDevice const &device);

    void _set_vertices(std::span<Vertex const> const vertices);
    void _set_indices(std::span<Index const> const indices);

private:
    vkDevice const &_device;

    vkBuffer *_vertex_buffer;
    std::vector<Vertex> _vertex_data;
    std::vector<vk::DeviceSize> _vertex_offsets;

    vkBuffer *_index_buffer;
    std::vector<Index> _index_data;
};

} // namespace btx


#endif // BRASSTACKS_ASSETS_MESHES_MESH_HPP