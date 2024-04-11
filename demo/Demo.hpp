#ifndef DEMO_HPP
#define DEMO_HPP

#include "brasstacks/brasstacks.hpp"

#include "brasstacks/math/Mat4.hpp"
#include "brasstacks/tools/cameras/CameraController.hpp"

class Demo final : public btx::Application {
public:
    Demo();
    ~Demo() override = default;

    void init() override;
    void shutdown() override;

    void update() override;
    void record_commands() override;

    void swapchain_updated() override;

    void create_pipeline() override;
    void destroy_pipeline() override;

private:
    btx::vkPipeline _pipeline;

    btx::MeshLibrary::MeshIter _plane_mesh;
    btx::math::Mat4            _plane_mat;

    btx::MeshLibrary::MeshIter _cube_mesh;
    btx::math::Mat4            _cube_mat;

    btx::TextureLibrary::TextureIter _texture;
};

#endif // DEMO_HPP