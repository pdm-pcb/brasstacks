#ifndef BRASSTACKS_ASSETS_MESHES_MESH_HPP
#define BRASSTACKS_ASSETS_MESHES_MESH_HPP

#include "brasstacks/pch.hpp"
#include "brasstacks/assets/meshes/Vertex.hpp"
#include "brasstacks/assets/meshes/Index.hpp"
#include "brasstacks/platform/vulkan/resources/vmaBuffer.hpp"

namespace btx {

class vkCmdBuffer;

class Mesh {
public:
    virtual ~Mesh() = default;

    void draw_indexed(vkCmdBuffer const &cmd_buffer) const;

    Mesh(Mesh &&) = delete;
    Mesh(const Mesh &) = delete;

    Mesh& operator=(Mesh &&) = delete;
    Mesh& operator=(Mesh const &) = delete;

protected:
    Mesh();

    void _set_vertices(std::span<Vertex const> const vertices);
    void _set_indices(std::span<Index const> const indices);

private:
    std::unique_ptr<vmaBuffer>  _vertex_buffer;
    std::vector<Vertex>         _vertex_data;
    std::vector<vk::DeviceSize> _vertex_offsets;

    std::unique_ptr<vmaBuffer> _index_buffer;
    std::vector<Index>         _index_data;
};

} // namespace btx


#endif // BRASSTACKS_ASSETS_MESHES_MESH_HPP