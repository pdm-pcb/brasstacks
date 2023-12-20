#ifndef BRASSTACKS_RENDERING_MESHES_MESH_HPP
#define BRASSTACKS_RENDERING_MESHES_MESH_HPP

#include "brasstacks/pch.hpp"
#include "brasstacks/rendering/meshes/Vertex.hpp"
#include "brasstacks/rendering/meshes/Index.hpp"

namespace btx {

class vkDevice;
class vkBuffer;
class vkCmdBuffer;

class Mesh {
public:
    void draw_indexed(vkCmdBuffer const &cmd_buffer) const;

    Mesh(Mesh &&) = delete;
    Mesh(const Mesh &) = delete;

    Mesh& operator=(Mesh &&) = delete;
    Mesh& operator=(Mesh const &) = delete;

protected:
    explicit Mesh(vkDevice const &device);
    ~Mesh();

    void _set_vertices(std::vector<Vertex> const &vertices);
    void _set_indices(std::vector<Index> const &indices);

private:
    vkDevice const &_device;

    vkBuffer           *_vertex_buffer;
    std::vector<Vertex> _vertex_data;
    std::vector<vk::DeviceSize> _vertex_offsets;

    vkBuffer          *_index_buffer;
    std::vector<Index> _index_data;
};

} // namespace btx


#endif // BRASSTACKS_RENDERING_MESHES_MESH_HPP